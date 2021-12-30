#include <stdio.h>
#include "base/lidig_plugin.h"


struct lidig_operations* ops_;
void* data_;

static int plugin_init(struct lidig_operations* ops, void* data)
{
    printf("init\n");
    ops_ = ops;
    data_ = data;
}

static void plugin_uninit(void)
{
    printf("uninit\n");
}

static int plugin_open(void)
{
    printf("open\n");
}

static int plugin_write(const void* data, size_t size)
{
    printf("write\n");
    ops_->read_cb(data_, (void*)data, size);
}

static int plugin_close(void)
{
    printf("close\n");
}

struct lidig_plugin lidig_plugin = {
    .init = plugin_init,
    .open = plugin_open,
    .write = plugin_write,
    .close = plugin_close,
    .uninit = plugin_uninit,
};