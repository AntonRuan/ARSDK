#include <string.h>
#include "base/lidig_logger.h"
#include "network/lidig_rpc.h"

using namespace std;

lidig_rpc::lidig_rpc() {
    LogTrace();
}

lidig_rpc::~lidig_rpc() {
    LogTrace();
}

void lidig_rpc::on_write(lidig_udp* udp, const char* data, int status) {

}

void lidig_rpc::on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) {
    LogDebug() << "from " << ip << ":" << port;
    jrpc_base jrpc(data, nread);
    if (jrpc.is_empty())
        return;

    LogInfo() << jrpc.to_string();
    if (jrpc.is_request()) {
        jrpc_request req(jrpc);
        if (func_map_.find(req.method()) != func_map_.end()) {
            Json::Value params = req.params();
            if (params.isArray() && params.size() > 1) {
                Json::Value result = func_map_[req.method()](params);
                jrpc_response res(req, result);
                async_write(ip, port, res.to_string());
            } else {
                jrpc_error err(req, JPRC_INVALID_REQUEST);
                async_write(ip, port, err.to_string());
            }
        } else {
            jrpc_error err(req, JPRC_METHOD_NOT_FOUND);
            string data = err.to_string();
            async_write(ip, port, err.to_string());
        }
    } else if (jrpc.is_response()) {
        jrpc_response res(jrpc);
        on_response(this, res.result());
    }
}
