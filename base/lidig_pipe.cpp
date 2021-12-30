#include "base/lidig_pipe.h"
#include "base/lidig_logger.h"

lidig_pipe::lidig_pipe() {
    LogTrace();
    handle_ = (uv_handle_t*) new uv_pipe_t();
    handle_->data = this;
}

lidig_pipe::~lidig_pipe() {
    delete(handle_);
    LogTrace();
}

int lidig_pipe::open(const std::string& file) {
    uv_fs_t file_req;

    fd_ = uv_fs_open(lidig_loop::get_loop(), &file_req, file.c_str(), O_RDWR, 0644, nullptr);//O_CREAT
    if (fd_ < 0) {
        LogWarn() << file << " failed!";
        return -1;
    }
    LogDebug() << file;

    if (uv_pipe_init(lidig_loop::get_loop(), (uv_pipe_t*) handle_, 0) != 0) {
        close();
        return -1;
    }

    if (uv_pipe_open((uv_pipe_t *) handle_, fd_) < 0) {
        close();
        return -1;
    }

    set_alive();
    return fd_;
}

void lidig_pipe::close() {
    if (fd_ <= 0)
        return;

    lidig_stream::close();
    uv_fs_t close_req;
    uv_fs_close(lidig_loop::get_loop(), &close_req, fd_, nullptr);
    fd_ = 0;
}

