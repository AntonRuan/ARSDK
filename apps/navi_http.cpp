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

#define HOST_WWW_PATH "./static"

void response_200(lidig_http_session* self) {
    string html = "<html><body><h1>Hello, World!</h1></body></html>\n";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");
}

void response_404(lidig_http_session* self) {
    self->send_response("404", "Not Found");
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

        lidig_http_session* session = (lidig_http_session*) file->user_data;
        session->add_body(data, nread);
        session->send_response("200", "OK");
    }
};

class navi_http : public lidig_http_server
{
public:
    virtual void on_packet(lidig_http_session* session,
                std::map<std::string,std::string> map, const char* data, ssize_t nread) override {
        LogInfo() << session->get_remote_ip() <<  " " << session->get_remote_port() <<  " " << map["Path"];

        if (map["Path"] != "") {
            string filename = string(HOST_WWW_PATH) + "/index.html";
            if (map["Path"] != "/")
                filename = HOST_WWW_PATH + map["Path"];
            test_file* file = new test_file();
            file->user_data = session;
            int fd = file->readfile(filename);
            if (fd < 0) {
                delete file;
                response_404(session);
            }
        } else {
            response_200(session);
            session->close();
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

    lidig_loop loop;

    navi_http http_server;
    http_server.listen("0.0.0.0", 10083);

    LogInfo() << "start!";

    return loop.run();
}