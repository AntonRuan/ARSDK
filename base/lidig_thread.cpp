#include "base/lidig_thread.h"
#include "base/lidig_logger.h"
#include "base/lidig_loop.h"

lidig_thread::lidig_thread() {
    LogTrace();
};

lidig_thread::~lidig_thread() {
    LogTrace();
};

int lidig_thread::create(void* arg, uv_thread_cb cb) {
    return uv_thread_create(&tid_, cb, arg);
}

int lidig_thread::join() {
    return uv_thread_join(&tid_);
}


lidig_async::lidig_async() {
    LogTrace();
};

lidig_async::~lidig_async() {
    LogTrace();
    uv_close((uv_handle_t*) &async_, nullptr);
};

void lidig_async::async_init(void* arg, lidig_async_cb cb) {
    user_data = arg;
    async_cb_ = cb;
    async_.data = this;
    uv_async_init(lidig_loop::get_loop(), &async_,
        [](uv_async_t *handle) {
            lidig_async* self = (lidig_async*) handle->data;
            if (self->async_cb_)
                self->async_cb_(self, self->user_data);
        }
    );
}

void lidig_async::async_send() {
    uv_async_send(&async_);
}


lidig_queue_work::lidig_queue_work() {
    LogTrace();
    ret_ = 0;
};

lidig_queue_work::~lidig_queue_work() {
    LogTrace();
};

int lidig_queue_work::work(void* arg,
            lidig_work_cb work_cb,
            lidig_after_work_cb after_work_cb) {
    user_data = arg;
    work_.data = this;
    work_cb_ = work_cb;
    after_work_cb_ = after_work_cb;

    return uv_queue_work(lidig_loop::get_loop(), &work_,
        [](uv_work_t* req) {
            lidig_queue_work* self = (lidig_queue_work*) req->data;
            if (self->work_cb_)
                self->ret_ = self->work_cb_(self->user_data);
        },
        [](uv_work_t* req, int status) {
            lidig_queue_work* self = (lidig_queue_work*) req->data;
            if (self->after_work_cb_)
                self->after_work_cb_(self->user_data, self->ret_);
        }
    );
}


lidig_mutex::lidig_mutex() {
    uv_mutex_init(&mutex_);
}

lidig_mutex::~lidig_mutex() {
    uv_mutex_destroy(&mutex_);
}

void lidig_mutex::lock() {
    uv_mutex_lock(&mutex_);
}

void lidig_mutex::unlock() {
    uv_mutex_unlock(&mutex_);
}


lidig_scope_lock::lidig_scope_lock(lidig_mutex* m)
            : m_(m) {
    LogTrace();
    m_->lock();
    is_locked_ = true;
}

lidig_scope_lock::~lidig_scope_lock() {
    unlock();
    LogTrace();
}

void lidig_scope_lock::lock() {
    if (!is_locked_) {
        m_->lock();
        is_locked_ = true;
    }
}

void lidig_scope_lock::unlock() {
    if (is_locked_) {
        m_->unlock();
        is_locked_ = false;
    }
}
