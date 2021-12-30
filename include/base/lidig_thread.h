/*
 *  Created on: 2020/8/21
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_THREAD_H__
#define __LIDIG_THREAD_H__

#include <uv.h>

class lidig_thread {
public:
    lidig_thread();
    ~lidig_thread();

    int create(void* arg, uv_thread_cb cb);
    int join(void);
    pthread_t get_thread_id(void) {return tid_;}

private:
    pthread_t tid_;
};


class lidig_async
{
public:
    lidig_async();
    ~lidig_async();

    typedef void (*lidig_async_cb)(lidig_async* async, void* arg);
    void async_init(void* arg, lidig_async_cb cb);
    void async_send();

protected:
    void* user_data;
    lidig_async_cb async_cb_;

private:
    uv_async_t async_;
};

class lidig_queue_work {
public:
    lidig_queue_work();
    ~lidig_queue_work();

    typedef int (*lidig_work_cb)(void* arg);
    typedef void (*lidig_after_work_cb)(void* arg, int status);
    int work(void* arg, lidig_work_cb work_cb,
                        lidig_after_work_cb after_work_cb);

protected:
    void* user_data;
    int ret_;
    lidig_work_cb work_cb_;
    lidig_after_work_cb after_work_cb_;

private:
    uv_work_t work_;
};


class lidig_mutex
{
public:
    lidig_mutex();
    ~lidig_mutex();

    void lock();
    void unlock();

private:
    uv_mutex_t mutex_;
};

class lidig_scope_lock
{
public:
    lidig_scope_lock(lidig_mutex* m_);
    ~lidig_scope_lock();

    void lock();
    void unlock();

    lidig_scope_lock(const lidig_scope_lock&) = delete;
    lidig_scope_lock& operator=(const lidig_scope_lock&) = delete;
private:
    lidig_mutex* m_;
    bool is_locked_;
};

#endif