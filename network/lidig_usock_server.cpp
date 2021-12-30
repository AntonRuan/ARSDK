#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include "network/lidig_usock_server.h"

lidig_usock_server::lidig_usock_server(lidig_loop *loop):loop_(loop) {
    handle_ = (uv_handle_t*) new uv_pipe_t();
    signal_ = new uv_signal_t();
}

lidig_usock_server::~lidig_usock_server() {
    delete(handle_);
    delete(signal_);
    uv_fs_t req;
    uv_fs_unlink(loop_->get_loop(), &req, sock_name_.c_str(), NULL);
}

void lidig_usock_server::remove_sock(uv_signal_t *handle, int sig) {
    lidig_usock_server* tmp_this = (lidig_usock_server*)handle->data;
    uv_fs_t req;
    uv_fs_unlink(tmp_this->loop_->get_loop(), &req, tmp_this->sock_name_.c_str(), NULL);
    uv_signal_stop(handle);
    exit(0);
}

int lidig_usock_server::bind(std::string sock_name) {
    int ret = -1;

    sock_name_ = sock_name;
    ret = uv_pipe_init(loop_->get_loop(), (uv_pipe_t*) handle_, 0);
    if (ret < 0)
        return ret;

    signal_->data = this;
    uv_signal_init(loop_->get_loop(), signal_);
    uv_signal_start(signal_, remove_sock, SIGINT);

    ret = uv_pipe_bind((uv_pipe_t*) handle_, sock_name_.c_str());
    return ret;
}

int lidig_usock_server::listen(connection_callback cb) {
    int ret = -1;

    handle_->data = this;
    ret = uv_listen((uv_stream_t*) handle_, 128, on_new_connection);
    if (ret == 0)
        connection_cb_ = cb;
    else
        close();

    return ret;
}

int lidig_usock_server::accept(lidig_tcp_client *tcp_client) {
    int ret = -1;

    uv_handle_t* client = tcp_client->get_handle();
    uv_tcp_init(loop_->get_loop(), (uv_tcp_t*) client);
    client->data = this;
    ret = uv_accept((uv_stream_t*) handle_, (uv_stream_t*) client);

    return ret;
}

void lidig_usock_server::on_new_connection(uv_stream_t *server, int status) {
    lidig_usock_server* self = (lidig_usock_server*)server->data;
    self->connection_cb_(self, status);
}