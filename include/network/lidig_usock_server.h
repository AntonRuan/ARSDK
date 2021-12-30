/*
 *  Created on: 2020/8/3
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_USOCK_SERVER_H__
#define __LIDIG_USOCK_SERVER_H__

#include <iostream>
#include <functional>
#include "base/lidig_loop.h"
#include "base/lidig_stream.h"
#include "lidig_tcp_client.h"

class lidig_usock_server : public lidig_stream
{
public:
    typedef std::function<void(lidig_usock_server*,int)> connection_callback;

    lidig_usock_server(lidig_loop* loop);
    ~lidig_usock_server();

    int bind(std::string sock_name);
    int listen(connection_callback cb);
    int accept(lidig_tcp_client *tcp_client);

protected:
    static void remove_sock(uv_signal_t *handle, int sig);
    static void on_new_connection(uv_stream_t *server, int status);

    connection_callback connection_cb_ = nullptr;

    lidig_loop* loop_;
    uv_signal_t* signal_;
    std::string sock_name_;
};

#endif