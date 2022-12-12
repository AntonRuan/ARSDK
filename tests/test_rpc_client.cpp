#include <string.h>
#include "base/lidig_cmd.h"
#include "base/lidig_loop.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_rpc.h"
#include "network/lidig_udp_discover.h"

using namespace std;


class rpc_timer: public lidig_timer {
public:
    virtual void on_rpc_time() {}
    virtual void on_time(lidig_timer* timer) override {
        on_rpc_time();
    }
};

class test_rpc: public lidig_rpc, public lidig_udp_discover, public rpc_timer
{
public:
    test_rpc() {
        id_ = 0;
    }

    void on_response(lidig_rpc* rpc, Json::Value result) override {
        LogInfo() << lidig_json::value_to_string(result);
    }

    void on_new_ip(std::string ip) override {
        LogInfo() << ip;
        ip_ = ip;
    }

    void on_delete_ip(std::string ip) override {
        LogInfo() << ip;
        ip_ = "";
    }

    int sum(int a, int b) {
        Json::Value params;
        params[0] = a;
        params[1] = b;
        jrpc_request req(id_, "sum", params);
        if (ip_ != "")
            return lidig_rpc::async_write(ip_, 7000, req.to_string());
        return 0;
    }

    int add(string a, string b) {
        Json::Value params;
        params[0] = a;
        params[1] = b;
        jrpc_request req(id_, "add", params);
        if (ip_ != "")
            return lidig_rpc::async_write(ip_, 7000, req.to_string());
        return 0;
    }

    void on_rpc_time() override {
        switch(id_) {
            case 0: {
                sum(1, 3);
            }
                break;
            case 1: {
                add("hello", "world");
            }
                break;
            default:
                id_ = -1;
                break;
        }
        id_++;
    }

private:
    int id_;
    string ip_;
};

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    test_rpc rpc;
    rpc.lidig_rpc::async_read_start();
    rpc.rpc_timer::timer_start(0, 2000);

    rpc.lidig_udp_discover::bind("0.0.0.0", 9999);
    rpc.lidig_udp_discover::async_read_start();

    return loop.run();
}