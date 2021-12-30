/*
 *  Created on: 2020/8/12
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_UDP_H__
#define __LIDIG_UDP_H__

#include <iostream>
#include <functional>
#include "base/lidig_loop.h"

class lidig_udp
{
public:
    typedef std::function<void(char*,int)> write_callback;
    typedef std::function<void(const char*,ssize_t,std::string,int)> read_callback;

    lidig_udp(lidig_loop* loop);
    ~lidig_udp();

    int bind(const std::string& ip, int port);

    int async_write(const std::string& ip, int port, const void* data, size_t size);
    int async_write(const std::string& ip, int port, const void* data, size_t size,
            write_callback cb);
    int async_write(const struct sockaddr *addr, const void* data, size_t size);
    int async_write(const struct sockaddr *addr, const void* data, size_t size,
            write_callback cb);
    int async_read_start(read_callback cb);
    void close();

    void* user_data;

protected:
    typedef struct {
        uv_udp_send_t req;
        uv_buf_t buf;
        write_callback write_cb;
    } write_req_t;

    static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void on_write(uv_udp_send_t *req, int status);
    static void on_read(uv_udp_t *req, ssize_t nread,
            const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);

    read_callback read_cb_ = nullptr;
    lidig_loop* loop_;
    uv_handle_t* handle_;
};

#endif
