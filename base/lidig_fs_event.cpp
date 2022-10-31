#include <sys/stat.h>
#include "base/lidig_fs_event.h"
#include "base/lidig_logger.h"

lidig_fs_event::lidig_fs_event() {
    LogTrace();
    handle_ = new uv_fs_event_t();
    handle_->data = this;
}

lidig_fs_event::~lidig_fs_event() {
    delete handle_;
    LogTrace();
}

int lidig_fs_event::init(const std::string& file) {
    uv_fs_event_init(lidig_loop::get_loop(), handle_);
    uv_fs_event_start(handle_,
        [](uv_fs_event_t *handle, const char *filename, int events, int status) {
            lidig_fs_event* self = (lidig_fs_event*) handle->data;
            self->on_changed(self, filename, events);
        },
    file.c_str(), UV_FS_EVENT_RECURSIVE);
    return 0;
}
