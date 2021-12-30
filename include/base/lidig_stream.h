/*
 *  Created on: 2020/8/6
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_STREAM_H__
#define __LIDIG_STREAM_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"

class lidig_stream
{
public:
    lidig_stream();
    ~lidig_stream();

    bool alive(void) {return is_alive_;}
    void set_alive(void) {is_alive_ = true;}
    void set_not_alive(void) {is_alive_ = false;}

    int async_write(const std::string& data);
    int async_write(const char* data, size_t size);
    int async_read_start(void);
    void close(void);

    uv_handle_t* get_handle(void);
    std::string get_local_ip(void);
    int get_local_port(void);
    std::string get_remote_ip(void);
    int get_remote_port(void);

    void* user_data;

protected:
    virtual void on_write(lidig_stream* stream, const char* data, int status) {}
    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) {}
    virtual void on_close(lidig_stream* stream) {}

    uv_handle_t* handle_;

private:
    typedef struct {
        uv_write_t req;
        uv_buf_t buf;
        const char* data;
    } write_req_t;
    bool is_alive_ = false;
};

#endif