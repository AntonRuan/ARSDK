/*
 *  Created on: 2020/8/7
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_DL_STREAM_H__
#define __LIDIG_DL_STREAM_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"
#include "lidig_plugin_common.h"

class lidig_plugin
{
public:
    typedef std::function<void(const char*,ssize_t)> read_callback;

    lidig_plugin();
    ~lidig_plugin();

    int init(const std::string name);
    int open();
    int write(const void* data, size_t size);
    void read_start(read_callback cb);
    static void on_read(void* handle, const char* data, ssize_t nread);
    int close();

protected:
    read_callback read_cb_ = nullptr;

    uv_lib_t *lib_;
    struct lidig_plugin_funcs *p_;
    struct lidig_plugin_cbs cbs_ = {
        .read_cb = on_read,
    };
};

#endif