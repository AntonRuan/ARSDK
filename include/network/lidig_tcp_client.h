/*
 *  Created on: 2020/8/5
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_TCP_CLIENT_H__
#define __LIDIG_TCP_CLIENT_H__

#include <iostream>
#include <functional>
#include "base/lidig_loop.h"
#include "base/lidig_stream.h"

class lidig_tcp_client : public lidig_stream
{
public:
    lidig_tcp_client();
    virtual ~lidig_tcp_client();

    int connect(const std::string host, int port);

protected:
    virtual void on_connection(lidig_tcp_client* client, int status) {}

    std::string host_;
    std::string ip_;
    int port_;

private:
    static void connect_cb(uv_connect_t* conn_req, int status);
    static void resolved_cb(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);

    uv_connect_t connect_req_;
    uv_getaddrinfo_t getaddrinfo_req_;
};

#endif