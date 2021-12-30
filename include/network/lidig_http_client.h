/*
 *  Created on: 2020/9/24
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_HTTP_CLIENT_H__
#define __LIDIG_HTTP_CLIENT_H__

#include <map>
#include <iostream>
#include <functional>
#include "util/http_parser.h"
#include "lidig_http_session.h"

class lidig_http_client : public lidig_http_session
{
public:
    lidig_http_client();
    ~lidig_http_client();

    int send_request(const std::string &method, const std::string &url);

protected:
    void on_connection(lidig_tcp_client* client, int status) final;
    virtual void on_connection(lidig_http_client* client, int status) {}
};

#endif