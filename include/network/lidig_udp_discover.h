/*
 *  Created on: 2022/11/17
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_UDP_DISCOVER_H__
#define __LIDIG_UDP_DISCOVER_H__

#include <iostream>
#include <functional>
#include "base/lidig_loop.h"
#include "base/lidig_timer.h"
#include "network/lidig_udp.h"
#include "protocol/jsonrpc.h"

class lidig_udp_discover: public lidig_udp, private lidig_timer
{
public:
    lidig_udp_discover();
    ~lidig_udp_discover();

    int async_read_start();

protected:
    virtual void on_response(lidig_udp_discover* rpc, Json::Value result) {}
    virtual void on_new_ip(std::string ip) {}
    virtual void on_delete_ip(std::string ip) {}

private:
    virtual void on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) final;
    virtual void on_time(lidig_timer* timer) final;

    typedef struct {
        int alive_count;
        int port;
    } ip_info_t;

    std::map<std::string, ip_info_t> ip_map_;
    std::string local_ip_;
};

#endif
