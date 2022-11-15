#include "network/lidig_udp.h"
#include "base/lidig_logger.h"
#include <string.h>

lidig_udp::lidig_udp() {
    LogTrace();
    handle_ = (uv_handle_t*) new uv_udp_t();
    uv_udp_init(lidig_loop::get_loop(), (uv_udp_t*) handle_);
    handle_->data = this;
}

lidig_udp::~lidig_udp() {
    delete(handle_);
    LogTrace();
}

int lidig_udp::bind(const std::string& ip, int port) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    return uv_udp_bind((uv_udp_t*) handle_, (const struct sockaddr*)&addr, 0);
}

void lidig_udp::on_write(uv_udp_send_t *req, int status) {
    lidig_udp* self = (lidig_udp*) req->data;
    write_req_t *wr = (write_req_t*) req;
    self->on_write(self, wr->buf.base, status);
    delete[] wr->buf.base;
    ::operator delete(wr);
}

int lidig_udp::async_write(const struct sockaddr *addr, const char* data, size_t size) {
    write_req_t* req = (write_req_t*)::operator new(sizeof(write_req_t));
    char* buf = new char[size];
    memcpy(buf, data, size);
    req->buf = uv_buf_init(buf, size);
    req->req.data = this;
    LogTrace().write_binary(data, size);
    return uv_udp_send((uv_udp_send_t*) req, (uv_udp_t*) handle_, &req->buf,
            1, (const struct sockaddr *) addr, on_write);
}

int lidig_udp::async_write(const std::string& ip, int port, const char* data, size_t size) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    return async_write((const struct sockaddr *) &addr, data, size);
}

void lidig_udp::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
}

void lidig_udp::on_read(uv_udp_t *handle, ssize_t nread,
        const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
    if (nread == 0) {
        delete[] buf->base;
        return;
    }

    if (nread < 0) {
        LogTrace() << "Read error " << uv_err_name(nread);
        return;
    }

    struct sockaddr_in *sock = ( struct sockaddr_in*) addr;
    int port = ntohs(sock->sin_port);
    struct in_addr in  = sock->sin_addr;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&in, ip, sizeof(ip));

    lidig_udp* tmp_this = (lidig_udp*)handle->data;
    tmp_this->on_read(tmp_this, buf->base, nread, ip, port);

    delete[] buf->base;
}

int lidig_udp::async_read_start() {
    return uv_udp_recv_start((uv_udp_t*) handle_, alloc_buffer, on_read);
}

void lidig_udp::close() {
    uv_udp_recv_stop((uv_udp_t*) handle_);
    uv_close(handle_, nullptr);
}
