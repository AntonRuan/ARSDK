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
    lidig_udp();
    ~lidig_udp();

    int bind(const std::string& ip, int port);

    int async_write(const std::string& ip, int port, const char* data, size_t size);
    int async_write(const struct sockaddr *addr, const char* data, size_t size);
    int async_read_start();
    void close();

    void* user_data;

protected:
    virtual void on_write(lidig_udp* udp, const char* data, int status) {}
    virtual void on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) {}

    typedef struct {
        uv_udp_send_t req;
        uv_buf_t buf;
    } write_req_t;

    static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void on_write(uv_udp_send_t *req, int status);
    static void on_read(uv_udp_t *req, ssize_t nread,
            const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);

    lidig_loop* loop_;
    uv_handle_t* handle_;
};

#endif
