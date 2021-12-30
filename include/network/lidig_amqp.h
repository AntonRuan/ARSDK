/*
 *  Created on: 2020/10/13
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_AMQP_H__
#define __LIDIG_AMQP_H__

#include <amqpcpp.h>
#include <amqpcpp/libuv.h>
#include "base/lidig_timer.h"
#include "base/lidig_health.h"

class lidig_amqp : public AMQP::LibUvHandler, public lidig_health
{
public:
    lidig_amqp();
    ~lidig_amqp();

    int connect(const std::string& address, const std::string& queue);
    void disconnect();

    void bind_queue(const std::string& key);
    void bind_queue(std::vector<std::string> v);

    void publish(const std::string& key, const std::string& message);
    void publish(const std::string& key, const char* data, size_t size);
    void publish(const std::string& exchange, const std::string& key, const std::string& message, const std::string& id);

protected:
    int health_cure();
    virtual void on_connection(lidig_amqp* amqp, int status) {}
    virtual std::string on_packet(const std::string&, const char*, size_t,
            const std::string&, const std::string&) {return "";}

private:
    virtual void onError(AMQP::TcpConnection *conn, const char *message) override;
    virtual void onConnected(AMQP::TcpConnection *conn) override;
    void heartbeat_cb();

    lidig_timer* timer_;
    std::string address_;
    std::string queue_name_;
    std::vector<std::string> key_vec_;
    AMQP::TcpChannel* channel_;
    AMQP::TcpConnection* connection_;
};

#endif