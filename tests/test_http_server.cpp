#include <unistd.h>
#include <vector>
#include "base/lidig_cmd.h"
#include "util/lidig_json.h"
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_http_server.h"
#include "network/lidig_tcp_client.h"

using namespace std;

#define HOST_WWW_PATH "/mnt/sda/www"

map<string, string> pictures_map_;

lidig_http_session* session_;


void response_200(lidig_http_session* self) {
    string html = "<html><body><h1>Hello, World!</h1></body></html>\n";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");
}

void response_404(lidig_http_session* self) {
    self->send_response("404", "Not Found");
}

void time_cb(lidig_timer* timer) {

}

class test_file : public lidig_file
{
public:
    virtual void on_write(lidig_file* file, const char* data) {

    }

    virtual void on_close(lidig_file* file) {
        delete file;
    }

    virtual void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        LogInfo() << nread;
        if (nread < 0)
            return;

        string path = file->get_filename();
        path = path.substr(path.rfind("/")+1);
        LogDebug() << path;
        pictures_map_[path] = string(data, nread);

        if (session_ == nullptr)
            return;

        session_->add_body(pictures_map_[path]);
        //session_->add_header("Content-Type", "image/jpeg");
        session_->send_response("200", "OK");
        //session_->close();
    }
};

class test_http_server : public lidig_http_server
{
public:
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {
        if (nread < 0)
            return;

        LogInfo() << session->get_remote_ip() <<  " " << session->get_remote_port() <<  " " << map["Path"];
        session_ = session;
        //string str(data, nread);
        //LogDebug() << nread << "\n" << str;
        if (map["Path"] == "/") {
            test_file* file = new test_file();
            string filename = "/index.html";
            int fd = file->readfile(HOST_WWW_PATH + filename);
            if (fd < 0) {
                delete file;
                response_404(session);
                session->close();
                return;
            }
        } else if (map["Path"] != "") {
            test_file* file = new test_file();
            int fd = file->readfile(HOST_WWW_PATH + map["Path"]);
            if (fd < 0) {
                delete file;
                response_404(session);
                session->close();
                return;
            }
        } else {
            response_200(session);
            session->close();
            return;
        }
    }

    virtual void on_close(lidig_http_session* session) {
        if (session_ == session) {
            session_ = nullptr;
        }
    }
};

int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;

    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "server", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop* loop_ = new lidig_loop();

    test_http_server* http_server = new test_http_server();
    http_server->listen("0.0.0.0", 10080);

    lidig_timer timer;
    timer.timer_start(1000, 60000, time_cb);

    LogInfo() << "start!";

    return loop_->run();
}