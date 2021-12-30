/*
 *  Created on: 2020/8/7
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_PLUGIN_COMMON_H__
#define __LIDIG_PLUGIN_COMMON_H__

struct lidig_plugin_cbs {
    void (*read_cb)(void* handle, const char* data, ssize_t nread);
};

struct lidig_plugin_funcs {
    int (*init)(struct lidig_plugin_cbs* cbs, void* handle);
    int (*open)(void);
    int (*write)(const void* data, size_t size);
    int (*close)(void);
    void (*uninit)(void);
};

#endif