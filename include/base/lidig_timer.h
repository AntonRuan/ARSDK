/*
 *  Created on: 2020/8/3
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_TIMER_H__
#define __LIDIG_TIMER_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"

class lidig_timer
{
public:
    typedef std::function<void(lidig_timer*)> timer_callback;

    lidig_timer();
    ~lidig_timer();

    void timer_start(uint64_t timeout, uint64_t repeat, timer_callback cb);
    void start();
    void stop();

    void* user_data;
    timer_callback timer_cb_ = nullptr;

private:
    uv_timer_t* timer_;
};

#endif