#include "base/lidig_logger.h"
#include "network/lidig_http_server.h"

using namespace std;

lidig_http_server::lidig_http_server() {
    LogTrace();
}

lidig_http_server::~lidig_http_server() {
    LogTrace();
}

void lidig_http_server::on_connection(lidig_tcp_server* self, int status) {
    LogTrace() << status;
    lidig_http_session* session = new lidig_http_session();
    session->set_http_server(this);

    if (accept(session) == 0)
        session->async_read_start();
    else {
        LogError() << "accept failed!";
        delete session;
    }
}

void lidig_http_server::on_close_session(lidig_http_session* session) {
    on_close(session);
    if (session)
        delete session;
}