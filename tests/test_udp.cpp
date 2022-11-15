#include <string.h>
#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_udp.h"
#include "protocol/zabbix.h"

using namespace std;

bool server_flag_ = true;
class test_udp : public lidig_udp, public lidig_timer, public zabbix_protocol
{
public:
    virtual void on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) override {
        LogDebug() << "data from " << ip << ":" << port;
        port_ = port;
        parser_packet(data, nread);
    }

    virtual void on_packet(const std::string& data, const std::string& reserved) override {
        LogDebug() << data.size() << ": " << data;
        if (!server_flag_) return;
        string senddata = encode_packet(data+"了", "NULL");
        int ret = async_write("127.0.0.1", port_, senddata.c_str(), senddata.size());
        LogDebug() << ret;
    }

    virtual void on_time(lidig_timer* timer) override {
        string data = encode_packet("点赞", "NULL");
        int ret = async_write("127.0.0.1", 7000, data.c_str(), data.size());
        LogDebug() << ret;
    }

private:
    int port_;
};

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('s') != cmd.end())
        server_flag_ = true;
    if (cmd.find('c') != cmd.end())
        server_flag_ = false;

    lidig_loop loop;

    test_udp udp_;

    if (server_flag_)
        udp_.bind("0.0.0.0", 7000);
    udp_.async_read_start();
    if (!server_flag_)
        udp_.timer_start(0, 5000);

    return loop.run();
}