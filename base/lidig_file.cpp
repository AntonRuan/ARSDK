#include <sys/stat.h>
#include "base/lidig_file.h"
#include "base/lidig_logger.h"

lidig_file::lidig_file() {
    LogTrace();
    handle_ = new uv_fs_t();
    handle_->data = this;
}

lidig_file::~lidig_file() {
    delete handle_;
    LogTrace();
}

int lidig_file::async_read(int64_t bytes, int64_t offset) {
    static std::function<void(uv_fs_t* req)> on_read;
    on_read = [](uv_fs_t* req) {
        lidig_file* self = (lidig_file*) req->data;
        self->on_read(self, self->read_buf_.base, req->result);
        uv_fs_req_cleanup(req);
        free(self->read_buf_.base);
    };

    read_buf_ = uv_buf_init((char*) malloc(bytes), bytes);
    return uv_fs_read(lidig_loop::get_loop(), handle_, fd_, &read_buf_, 1, offset, [](uv_fs_t* req) {
        on_read(req);
    });
}

int lidig_file::async_write(const char* data, size_t size, int64_t offset) {
    write_req_t* req = (write_req_t*)::operator new(sizeof(write_req_t));

    req->data = data;
    char* buf = new char[size];
    memcpy(buf, data, size);
    req->buf = uv_buf_init(buf, size);

    static std::function<void(uv_fs_t* req)> on_write;
    on_write = [](uv_fs_t* req) {
        lidig_file* self = (lidig_file*) req->data;
        write_req_t *wr = (write_req_t*) req;
        self->on_write(self, wr->data);
        delete[] wr->buf.base;
        ::operator delete(wr);
    };

    req->req.data = this;
    return uv_fs_write(lidig_loop::get_loop(), (uv_fs_t*)req, fd_, &req->buf, 1, offset, [](uv_fs_t* req) {
        on_write(req);
    });
}

int lidig_file::open(const std::string& file, int flags) {
    file_name_ = file;
    fd_ = uv_fs_open(lidig_loop::get_loop(), handle_, file.c_str(), O_RDONLY | flags, 0644, nullptr);
    if (fd_ < 0)
        LogError() << file << " failed!";
    else
        LogDebug() << file;
    return fd_;
}

int lidig_file::readfile(const std::string& file) {
    int ret = open(file, O_RDONLY);
    if (ret < 0)
        return ret;

    struct stat buf;
    stat(file_name_.c_str(), &buf);
    file_size_ = buf.st_size;
    file_offset_ = 0;
    return_read_char_ = (char*) malloc(file_size_);
    if (return_read_char_ == nullptr)
        LogError() << "malloc size:" << file_size_ << " failed";

    static std::function<void(uv_fs_t* req)> on_read;
    on_read = [](uv_fs_t* req) {
        lidig_file* self = (lidig_file*) req->data;
        if (req->result <= 0) {
            LogError() << "File read failed";
            uv_fs_req_cleanup(req);
            free(self->read_buf_.base);
            self->close();
            free(self->return_read_char_);
            return;
        }

        memcpy(self->return_read_char_ + self->file_offset_,
                self->read_buf_.base, req->result);
        self->file_offset_ += req->result;

        uv_fs_req_cleanup(req);
        free(self->read_buf_.base);

        if (self->file_offset_ < self->file_size_) {
            self->read_buf_ = uv_buf_init((char*) malloc(1024), 1024);
            uv_fs_read(lidig_loop::get_loop(), self->handle_, self->fd_,
                &self->read_buf_, 1, self->file_offset_,
                [](uv_fs_t* req) {
                    on_read(req);
                }
            );
        } else {
            self->on_read(self, self->return_read_char_, self->file_offset_);
            self->close();
            free(self->return_read_char_);
        }
    };

    read_buf_ = uv_buf_init((char*) malloc(1024), 1024);
    uv_fs_read(lidig_loop::get_loop(), handle_, fd_,
        &read_buf_, 1, file_offset_,
        [](uv_fs_t* req) {
            on_read(req);
        }
    );

    return ret;
}

void lidig_file::close() {
    if (fd_ <= 0)
        return;

    static std::function<void(uv_fs_t*)> on_close;
    on_close = [](uv_fs_t* req) {
        lidig_file* self = (lidig_file*) req->data;
        self->on_close(self);
    };

    LogDebug() << file_name_;
    uv_fs_close(lidig_loop::get_loop(), handle_, fd_,
        [](uv_fs_t* req) {
            on_close(req);
        }
    );
    fd_ = 0;
}
