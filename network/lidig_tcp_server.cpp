#include "base/lidig_logger.h"
#include "network/lidig_tcp_server.h"

lidig_tcp_server::lidig_tcp_server() {
    LogTrace();
    handle_ = (uv_handle_t*) new uv_tcp_t();
    handle_->data = this;
}

lidig_tcp_server::~lidig_tcp_server() {
    delete(handle_);
    LogTrace();
}

int lidig_tcp_server::accept(lidig_tcp_client *tcp_client) {
    uv_handle_t* client = tcp_client->get_handle();
    uv_tcp_init(lidig_loop::get_loop(), (uv_tcp_t*) client);
    int ret = uv_accept((uv_stream_t*) handle_, (uv_stream_t*) client);
    if (ret < 0) {
        LogError() << "Accpet error: " << uv_err_name(ret);
        return ret;
    }
    tcp_client->set_alive();
    on_accept(tcp_client);
    return ret;
}

void lidig_tcp_server::on_new_connection(uv_stream_t *server, int status) {
    lidig_tcp_server* self = (lidig_tcp_server*)server->data;
    if (status < 0)
        LogError() << "New connection error: " << uv_strerror(status);

    self->on_connection(self, status);
}

int lidig_tcp_server::listen(std::string ip, int port) {
    int ret = -1;

    LogInfo() << ip << " " << port;
    ret = uv_tcp_init(lidig_loop::get_loop(), (uv_tcp_t*) handle_);
    if (ret < 0) {
        LogError() << ip << " " << port << " Init error: " << uv_err_name(ret);
        return ret;
    }

    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    ret = uv_tcp_bind((uv_tcp_t*) handle_, (const struct sockaddr*)&addr, 0);
    if (ret < 0) {
        LogError() << ip << " " << port << " Bind error: " << uv_err_name(ret);
        return ret;
    }

    ret = uv_listen((uv_stream_t*) handle_, 128, on_new_connection);
    if (ret < 0) {
        LogError() << ip << " " << port << " Listen error: " << uv_err_name(ret);
        close();
    }

    return ret;
}