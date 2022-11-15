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
    lidig_timer();
    ~lidig_timer();

    void timer_start(uint64_t timeout, uint64_t repeat);
    void start();
    void stop();

    void* user_data;

protected:
    virtual void on_time(lidig_timer* timer) {}

private:
    uv_timer_t* timer_;

    static void time_cb(uv_timer_t *handle);
};

#endif