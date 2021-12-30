#include "base/lidig_stream.h"
#include "base/lidig_logger.h"
#include <string.h>

lidig_stream::lidig_stream() {
    LogTrace();
}

lidig_stream::~lidig_stream() {
    LogTrace();
}

uv_handle_t* lidig_stream::get_handle(void) {
    return handle_;
}

int lidig_stream::async_write(const std::string& data) {
    return async_write(data.data(), data.size());
}

int lidig_stream::async_write(const char* data, size_t size) {
    if (!alive() || data == nullptr) {
        on_write(this, data, -1);
        return -1;
    }
    LogTrace().write_binary(data, size);

    write_req_t* req = (write_req_t*)::operator new(sizeof(write_req_t));
    req->req.data = this;
    req->data = data;
    char* buf = new char[size];
    memcpy(buf, data, size);
    req->buf = uv_buf_init(buf, size);

    static std::function<void(uv_write_t *req, int status)> on_write;
    on_write = [](uv_write_t *req, int status) {
        lidig_stream* self = (lidig_stream*) req->data;
        write_req_t *wr = (write_req_t*) req;
        self->on_write(self, wr->data, status);
        delete[] wr->buf.base;
        ::operator delete(wr);
    };

    return uv_write((uv_write_t*) req, (uv_stream_t*) handle_, &req->buf, 1,
        [](uv_write_t *req, int status) {
            on_write(req, status);
        }
    );
}

int lidig_stream::async_read_start(void) {
    static std::function<void(uv_handle_t*, size_t, uv_buf_t*)> on_alloc_buffer;
    on_alloc_buffer = [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
        buf->base = new char[suggested_size];
        buf->len = suggested_size;
    };

    static std::function<void(uv_stream_t*, ssize_t, const uv_buf_t*)> on_read;
    on_read = [](uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
        if (nread == 0) {
            delete[] buf->base;
            return;
        }

        lidig_stream* self = (lidig_stream*) client->data;
        if (nread < 0) {
            LogTrace() << "Read error " << uv_err_name(nread);
            self->set_not_alive();
            self->close();
        } else {
            self->on_read(self, buf->base, nread);
        }
        delete[] buf->base;
    };

    set_alive();
    return uv_read_start((uv_stream_t*) handle_,
        [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
            on_alloc_buffer(handle, suggested_size, buf);
        },
        [](uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
            on_read(client, nread, buf);
        }
    );
}

void lidig_stream::close(void) {
    if (uv_is_closing(handle_))
        return;

    set_not_alive();
    uv_read_stop((uv_stream_t*) handle_);

    static std::function<void(uv_handle_t*)> on_close;
    on_close = [](uv_handle_t *handle) {
        lidig_stream* self = (lidig_stream*) handle->data;
        self->on_close(self);
    };
    uv_close(handle_,
        [](uv_handle_t *handle) {
            on_close(handle);
        }
    );
}

std::string lidig_stream::get_local_ip(void) {
    sockaddr_storage addr;
    int addr_len = sizeof(addr);
    uv_tcp_getsockname((uv_tcp_t*)handle_, (sockaddr*)&addr, &addr_len);

    char endpoint[64] = { 0 };
    if (addr.ss_family == AF_INET) {
        uv_ip4_name((const sockaddr_in*)&addr, endpoint, 64);
    }
    else if (addr.ss_family == AF_INET6) {
        uv_ip6_name((const sockaddr_in6*)&addr, endpoint, 64);
    }

    return endpoint;
}

int lidig_stream::get_local_port(void) {
    sockaddr_storage addr;
    int addr_len = sizeof(addr);
    uv_tcp_getsockname((uv_tcp_t*)handle_, (sockaddr*)&addr, &addr_len);

    if (addr.ss_family == AF_INET) {
        sockaddr_in *s = (sockaddr_in*)&addr;
        return ntohs(s->sin_port);
    }
    else if (addr.ss_family == AF_INET6) {
        sockaddr_in6 *s = (sockaddr_in6*)&addr;
        return ntohs(s->sin6_port);
    }

    return 0;
}

std::string lidig_stream::get_remote_ip(void) {
    sockaddr_storage addr;
    int addr_len = sizeof(addr);
    uv_tcp_getpeername((uv_tcp_t*)handle_, (sockaddr*)&addr, &addr_len);

    char endpoint[64] = { 0 };
    if (addr.ss_family == AF_INET) {
        uv_ip4_name((const sockaddr_in*)&addr, endpoint, 64);
    }
    else if (addr.ss_family == AF_INET6) {
        uv_ip6_name((const sockaddr_in6*)&addr, endpoint, 64);
    }

    return endpoint;
}

int lidig_stream::get_remote_port(void) {
    sockaddr_storage addr;
    int addr_len = sizeof(addr);
    uv_tcp_getpeername((uv_tcp_t*)handle_, (sockaddr*)&addr, &addr_len);

    if (addr.ss_family == AF_INET) {
        sockaddr_in *s = (sockaddr_in*)&addr;
        return ntohs(s->sin_port);
    }
    else if (addr.ss_family == AF_INET6) {
        sockaddr_in6 *s = (sockaddr_in6*)&addr;
        return ntohs(s->sin6_port);
    }

    return 0;
}