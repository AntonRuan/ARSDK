#include "base/lidig_cmd.h"
#include "base/lidig_file.h"
#include "base/lidig_logger.h"
#include "network/lidig_http_client.h"

using namespace std;

lidig_http_client* http_client_;
lidig_file* file_;
string picture_;
lidig_loop* loop_;


class test_http_client : public lidig_http_client
{
public:
    void on_connection(lidig_http_client* client, int status) {
        if (status < 0)
            return;
        LogInfo();
        add_header("Content-Type", "jpg");
        add_body(picture_);
        send_request("POST", "/upload");
    }

    void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {
        if (nread < 0)
            return;

        LogInfo() << session->get_url()  << " " << map["Status"] << " " << map["Reason"];
        string str(data, nread);
        LogDebug() << str.size() << "\n" << str;
        session->close();
    }
};

class test_file : public lidig_file
{
public:
    virtual void on_write(lidig_file* file, const char* data) {

    }

    virtual void on_close(lidig_file* file) {

    }

    virtual void on_read(lidig_file* file, char* data, ssize_t nread) {
        if (nread < 0)
            return;

        picture_.append(data, nread);
    }
};

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();


    loop_ = new lidig_loop();
    file_ = new test_file();
    file_->readfile("/home/ruan/temp/faceImage.jpg");

    http_client_ = new test_http_client();
    http_client_->connect("192.168.90.230", 4869);

    //lidig_http_client http_client;
    //http_client.send_request("GET", "http://192.168.90.230:4869");

    LogInfo() << "start!";

    return loop_->run();
}