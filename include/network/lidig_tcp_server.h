/*
 *  Created on: 2020/8/3
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_TCP_SERVER_H__
#define __LIDIG_TCP_SERVER_H__

#include <iostream>
#include <functional>
#include "base/lidig_stream.h"
#include "lidig_tcp_client.h"

class lidig_tcp_server : public lidig_stream
{
public:
    lidig_tcp_server();
    ~lidig_tcp_server();

    int listen(std::string ip, int port);
    int accept(lidig_tcp_client *tcp_client);

protected:
    virtual void on_accept(lidig_tcp_client* client) {}
    virtual void on_connection(lidig_tcp_server* self, int status) {}

private:
    static void on_new_connection(uv_stream_t *server, int status);
};

#endif