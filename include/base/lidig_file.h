/*
 *  Created on: 2020/10/16
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_FILE_H__
#define __LIDIG_FILE_H__

#include <iostream>
#include <functional>
#include "lidig_loop.h"
#include "lidig_stream.h"

class lidig_file
{
public:
    lidig_file();
    virtual ~lidig_file();

    int open(const std::string& file, int flags = 0);
    int async_read(int64_t bytes, int64_t offset);
    int async_write(const char* data, size_t size, int64_t offset);
    void close();

    int get_fd() {return fd_;};
    int64_t get_filesize() {return file_size_;};
    std::string get_filename() {return file_name_;};

    int readfile(const std::string& file);

    void* user_data;

protected:
    virtual void on_write(lidig_file* file, const char* data) {}
    virtual void on_close(lidig_file* file) {}
    virtual void on_read(lidig_file* file, const char* data, ssize_t nread) {}

    typedef struct {
        uv_fs_t req;
        uv_buf_t buf;
        const char* data;
    } write_req_t;

    uv_fs_t* handle_;
    int fd_;
    int64_t file_offset_;
    int64_t file_size_;
    std::string file_name_;
    uv_buf_t read_buf_;
    char* return_read_char_;
};

#endif