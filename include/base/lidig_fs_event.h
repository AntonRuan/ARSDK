/*
 *  Created on: 2021/12/29
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_FS_EVENT_H__
#define __LIDIG_FS_EVENT_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"
#include "lidig_stream.h"

class lidig_fs_event
{
public:
    lidig_fs_event();
    ~lidig_fs_event();

    int init(const std::string& file);

protected:
    virtual void on_changed(lidig_fs_event* file, const char* filename, int events) {}

private:
    uv_fs_event_t* handle_;
};

#endif