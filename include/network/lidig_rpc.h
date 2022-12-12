/*
 *  Created on: 2022/11/16
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_RPC_H__
#define __LIDIG_RPC_H__

#include <iostream>
#include <functional>
#include "base/lidig_loop.h"
#include "network/lidig_udp.h"
#include "protocol/jsonrpc.h"

class lidig_rpc: public lidig_udp
{
public:
    lidig_rpc();
    ~lidig_rpc();

    void add_func(std::string method, std::function<Json::Value(Json::Value)> func) {
        func_map_[method] = func;
    }
protected:
    virtual void on_response(lidig_rpc* rpc, Json::Value result) {}

private:
    virtual void on_write(lidig_udp* udp, const char* data, int status) override;
    virtual void on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) override;

    std::map<std::string, std::function<Json::Value(Json::Value)>> func_map_;
};

#endif
