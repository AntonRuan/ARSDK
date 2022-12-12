#include <string.h>
#include "base/lidig_cmd.h"
#include "base/lidig_logger.h"
#include "network/lidig_udp_discover.h"

using namespace std;


class test_udp_discover: public lidig_udp_discover
{
public:
    void on_response(lidig_udp_discover* rpc, Json::Value result) override {

    }

    void on_new_ip(std::string ip) override {
        LogInfo() << ip;
    }

    void on_delete_ip(std::string ip) override {
        LogInfo() << ip;
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


    lidig_loop loop;

    test_udp_discover discover;
    discover.bind("0.0.0.0", 9999);
    discover.async_read_start();

    return loop.run();
}