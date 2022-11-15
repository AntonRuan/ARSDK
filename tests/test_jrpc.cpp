#include <string.h>
#include "base/lidig_cmd.h"
#include "base/lidig_loop.h"
#include "base/lidig_logger.h"
#include "protocol/jsonrpc.h"

using namespace std;

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    map<char, string> cmd = lidig_cmd(argc, argv);

    lidig_loop loop;

    Json::Value params;
    params[0] = 1;
    params[1] = 2;
    jrpc_request req(9, "add", params);

    string req_data = req.to_string();
    LogInfo() << "--> " << req_data;

    jrpc_response res(req, 0);
    string res_data = res.to_string();
    LogInfo() << "<-- " << res_data;

    string data = R"({"jsonrpc": "2.0", "method": "sum", "params": [1, 2, 3, 4, 5], "id": 1})";
    jrpc_base jrpc(data);
    if (jrpc.is_request()) {
        jrpc_request req(jrpc);
        LogInfo() << "--> " << req.to_string();
        LogDebug() << req.id();
        LogDebug() << req.method();
        Json::Value params = req.params();
        LogDebug() << lidig_json::value_to_string(params);
    } else if (jrpc.is_response()) {
        jrpc_response res(jrpc);
        LogInfo() << "<-- " << res.to_string();
        LogDebug() << res.id();
        LogDebug() << res.result();
    }

    return loop.run();
}