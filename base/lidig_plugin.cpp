#include "base/lidig_plugin.h"

lidig_plugin::lidig_plugin() {
    lib_ = new uv_lib_t();
}

lidig_plugin::~lidig_plugin() {
    delete(lib_);
}

int lidig_plugin::init(const std::string name) {
    if (uv_dlopen(name.c_str(), lib_))
        return -1;

    if (uv_dlsym(lib_, "lidig_plugin", (void **) &p_))
        return -2;

    if (p_ == nullptr || p_->init == nullptr)
        return -3;

    return p_->init(&cbs_, this);
}

void lidig_plugin::on_read(void* handle, const char* data, ssize_t size) {
    if (!handle)
        return;

    lidig_plugin* tmp_this = (lidig_plugin*) handle;
    if (tmp_this->read_cb_)
        tmp_this->read_cb_(data, size);
}

int lidig_plugin::open() {
    if (p_ == nullptr || p_->open == nullptr)
        return -1;
    return p_->open();
}

int lidig_plugin::write(const void* data, size_t size) {
    if (p_ == nullptr || p_->write == nullptr)
        return -1;
    return p_->write(data, size);
}

void lidig_plugin::read_start(read_callback cb) {
    read_cb_ = cb;
}

int lidig_plugin::close() {
    if (p_ == nullptr || p_->close == nullptr)
        return -1;
    return p_->close();
}
