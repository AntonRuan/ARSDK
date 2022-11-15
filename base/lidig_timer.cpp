#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

lidig_timer::lidig_timer() {
    LogTrace();
    timer_ = new uv_timer_t();
}

lidig_timer::~lidig_timer() {
    stop();
    delete(timer_);
    LogTrace();
}

void lidig_timer::time_cb(uv_timer_t *handle) {
    lidig_timer* self = (lidig_timer*) handle->data;
    self->on_time(self);
}

void lidig_timer::timer_start(uint64_t timeout, uint64_t repeat) {
    uv_timer_init(lidig_loop::get_loop(), timer_);
    timer_->data = this;
    uv_timer_start(timer_, time_cb, timeout, repeat);
}

void lidig_timer::start() {
    uv_timer_again(timer_);
}

void lidig_timer::stop() {
    uv_timer_stop(timer_);
}