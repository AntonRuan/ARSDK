#include "base/lidig_loop.h"

lidig_loop::lidig_loop() {
    uv_loop_ = uv_default_loop();
};

lidig_loop::~lidig_loop() {
};

int lidig_loop::run() {
    return uv_run(uv_loop_, UV_RUN_DEFAULT);
};

uv_loop_t *lidig_loop::get_loop() {
    return uv_default_loop();
};