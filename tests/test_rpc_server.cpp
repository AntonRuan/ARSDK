#include <string.h>
#include "base/lidig_cmd.h"
#include "base/lidig_loop.h"
#include "base/lidig_logger.h"
#include "network/lidig_rpc.h"
#include "network/lidig_udp_discover.h"

using namespace std;

class test_rpc: public lidig_rpc, public lidig_udp_discover
{
public:
    static Json::Value sum(Json::Value params) {
        return params[0].asInt() + params[1].asInt();
    }

    static Json::Value add(Json::Value params) {
        return params[0].asString() + " " + params[1].asString();
    }
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
    rpc.add_func("sum", test_rpc::sum);
    rpc.add_func("add", test_rpc::add);
    rpc.lidig_rpc::bind("0.0.0.0", 7000);
    rpc.lidig_rpc::async_read_start();

    rpc.lidig_udp_discover::bind("0.0.0.0", 9999);
    rpc.lidig_udp_discover::async_read_start();

    return loop.run();
}