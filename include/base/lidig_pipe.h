/*
 *  Created on: 2020/8/5
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_PIPE_H__
#define __LIDIG_PIPE_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"
#include "lidig_stream.h"

class lidig_pipe : public lidig_stream
{
public:
    lidig_pipe();
    virtual ~lidig_pipe();

    int open(const std::string& file);
    void close();
    int get_fd() {return fd_;};

protected:
    int fd_ = 0;
};

#endif