#include "network/lidig_udp.h"
#include <string.h>

lidig_udp::lidig_udp(lidig_loop *loop):loop_(loop) {
    handle_ = (uv_handle_t*) new uv_udp_t();
    uv_udp_init(loop_->get_loop(), (uv_udp_t*) handle_);
}

lidig_udp::~lidig_udp() {
    delete(handle_);
}

int lidig_udp::bind(const std::string& ip, int port) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    return uv_udp_bind((uv_udp_t*) handle_, (const struct sockaddr*)&addr, 0);
}

void lidig_udp::on_write(uv_udp_send_t *req, int status) {
    write_req_t *wr = (write_req_t*) req;
    if (wr->write_cb)
        wr->write_cb(wr->buf.base, status);
    else
        delete[] wr->buf.base;
    ::operator delete(wr);
}

int lidig_udp::async_write(const struct sockaddr *addr, const void* data, size_t size) {
    write_req_t* req = (write_req_t*)::operator new(sizeof(write_req_t));
    char* buf = new char[size];
    memcpy(buf, data, size);
    req->buf = uv_buf_init(buf, size);
    return uv_udp_send((uv_udp_send_t*) req, (uv_udp_t*) handle_, &req->buf,
            1, (const struct sockaddr *) addr, on_write);
}

int lidig_udp::async_write(const struct sockaddr *addr, const void* data, size_t size,
        write_callback cb) {
    write_req_t* req = (write_req_t*)::operator new(sizeof(write_req_t));
    req->buf = uv_buf_init((char*)data, size);
    req->write_cb = cb;
    return uv_udp_send((uv_udp_send_t*) req, (uv_udp_t*) handle_, &req->buf,
            1, (const struct sockaddr *) addr, on_write);
}

int lidig_udp::async_write(const std::string& ip, int port, const void* data, size_t size) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    return async_write((const struct sockaddr *) &addr, data, size);
}

int lidig_udp::async_write(const std::string& ip, int port, const void* data, size_t size,
        write_callback cb) {
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    return async_write((const struct sockaddr *) &addr, data, size, cb);
}

void lidig_udp::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
}

void lidig_udp::on_read(uv_udp_t *handle, ssize_t nread,
        const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
    if (nread <= 0)
        return;

    struct sockaddr_in *sock = ( struct sockaddr_in*) addr;
    int port = ntohs(sock->sin_port);
    struct in_addr in  = sock->sin_addr;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&in, ip, sizeof(ip));

    lidig_udp* tmp_this = (lidig_udp*)handle->data;
    if (tmp_this->read_cb_)
        tmp_this->read_cb_(buf->base, nread, ip, port);

    delete[] buf->base;
}

int lidig_udp::async_read_start(read_callback cb) {
    read_cb_ = cb;
    handle_->data = this;
    return uv_udp_recv_start((uv_udp_t*) handle_, alloc_buffer, on_read);
}

void lidig_udp::close() {
    uv_udp_recv_stop((uv_udp_t*) handle_);
    uv_close(handle_, nullptr);
}
