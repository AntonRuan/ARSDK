#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_file.h"
#include "base/lidig_logger.h"
#include "network/lidig_http_client.h"

using namespace std;

class test_http_client : public lidig_http_client
{
public:
    virtual void on_connection(lidig_http_client* client, int status) {
        if (status < 0)
            return;
        LogInfo();
        add_header("Authorization", "123");
        send_request("GET", "/ISAPI/Event/notification/alertStream");
    }

    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> header_map, const char* data, ssize_t nread) {
        if (nread < 0)
            return;

        LogDebug() << get_url();
        LogInfo() << get_remote_port() << " " << header_map["Status"] << " " << header_map["Reason"];
        string str(data, nread);
        LogDebug() << str.size() << "\n" << str;
        close();
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
    lidig_logger::get_instance().add_logger_to_file("/tmp", "client", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    for (int i = 0; i < 100; ++i) {
        lidig_http_client* http_client = new test_http_client();
        http_client->connect("192.168.90.230", 20000+i);
    }

    lidig_timer timer;
    timer.timer_start(1000, 100000,
        [&](lidig_timer*) {
            //http_client.send_request("GET", "/ISAPI/System/capabilities?format=json");
            timer.stop();
        }
    );

    LogInfo() << "start!";

    return loop.run();
}