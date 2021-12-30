/*
 *  Created on: 2020/8/3
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_LOOP_H__
#define __LIDIG_LOOP_H__

#include <uv.h>

class lidig_loop {
public:
    lidig_loop();
    ~lidig_loop();

    int run();
    static uv_loop_t *get_loop();

private:
    uv_loop_t *uv_loop_;
};

#endif