#include "base/lidig_logger.h"
#include "network/lidig_tcp_client.h"


lidig_tcp_client::lidig_tcp_client() {
    LogTrace();
    handle_ = (uv_handle_t*) new uv_tcp_t();
    handle_->data = this;
    connect_req_.data = this;
    getaddrinfo_req_.data = this;
}

lidig_tcp_client::~lidig_tcp_client() {
    delete(handle_);
    LogTrace();
}

void lidig_tcp_client::connect_cb(uv_connect_t* conn_req, int status) {
    lidig_tcp_client* self = (lidig_tcp_client*) conn_req->data;

    if (status < 0) {
        LogError() << self->ip_ << ":" << self->port_ << " " << uv_strerror(status);
        self->set_not_alive();
        self->close();
        self->on_connection(self, status);
        return;
    }

    if (self->host_ == self->ip_)
        LogDebug() << self->ip_ << ":" << self->port_ << " Connected!";
    else
        LogDebug() << self->host_ << "(" << self->ip_ << "):" << self->port_ << " Connected!";
    self->async_read_start();
    self->on_connection(self, status);
}

void lidig_tcp_client::resolved_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) {
    lidig_tcp_client* self = (lidig_tcp_client*) resolver->data;

    if (status < 0) {
        LogError() << "getaddrinfo callback error " << uv_err_name(status);
        self->on_close(self);
        return;
    }

    char addr[17] = {'\0'};
    uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 16);
    self->ip_ = std::string(addr);
    LogDebug() << self->host_ << " dns (" << self->ip_ << ")";

    int ret = uv_tcp_init(lidig_loop::get_loop(), (uv_tcp_t*) self->handle_);
    if (ret < 0) {
        uv_freeaddrinfo(res);
        LogError() << self->host_ << " uv_tcp_init error " << uv_err_name(ret);
        return;
    }

    ret = uv_tcp_connect(&self->connect_req_, (uv_tcp_t*) self->handle_,
            (const struct sockaddr*) res->ai_addr, self->connect_cb);
    if (ret < 0) {
        uv_freeaddrinfo(res);
        LogError() << self->host_ << " uv_tcp_connect error " << uv_err_name(ret);
        return;
    }

    uv_freeaddrinfo(res);
}

int lidig_tcp_client::connect(const std::string host, int port) {
    host_ = host;
    ip_ = host;
    port_ = port;
    LogDebug() << "ing to "<< host << ":" << port;
    std::string p = std::to_string(port);
    int ret = -1;

    struct sockaddr_in sa;
    ret = inet_pton(AF_INET, host.data(), &(sa.sin_addr));
    // 域名还是IP
    if (ret != 0) {
        ret = uv_tcp_init(lidig_loop::get_loop(), (uv_tcp_t*) handle_);
        if (ret < 0) {
            LogError() << host_ << " uv_tcp_init error " << uv_err_name(ret);
            return ret;
        }

        struct sockaddr_in addr;
        uv_ip4_addr(host.data(), port, &addr);
        ret = uv_tcp_connect(&connect_req_, (uv_tcp_t*) handle_,
                (const struct sockaddr*) &addr, connect_cb);
        if (ret < 0) {
            LogError() << host_ << " uv_tcp_connect error " << uv_err_name(ret);
            return ret;
        }
    } else {
        // 域名查询开启4个线程，回调函数还是在主线程中执行
        ret = uv_getaddrinfo(lidig_loop::get_loop(), &getaddrinfo_req_, resolved_cb,
                host.data(), p.data(), nullptr);
        if (ret < 0) {
            LogError() << host << " getaddrinfo call error " << uv_err_name(ret);
            return ret;
        }
    }

    return ret;
}