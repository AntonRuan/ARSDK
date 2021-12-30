/*
 *  Created on: 2020/9/30
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_HTTP_SERVER_H__
#define __LIDIG_HTTP_SERVER_H__

#include <map>
#include <iostream>
#include <functional>
#include "lidig_http_session.h"
#include "lidig_tcp_server.h"

class lidig_http_session;
class lidig_http_server : public lidig_tcp_server
{
public:
    lidig_http_server();
    virtual ~lidig_http_server();

    void on_close_session(lidig_http_session* session);
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {}

protected:
    virtual void on_connection(lidig_tcp_server* self, int status) override;
    virtual void on_close(lidig_http_session* session) {}
};

#endif