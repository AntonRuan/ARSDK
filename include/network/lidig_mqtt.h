/*
 *  Created on: 2020/11/19
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_MQTT_H__
#define __LIDIG_MQTT_H__

#include <mosquittopp.h>
#include "base/lidig_health.h"
#include "base/lidig_thread.h"

class lidig_mqtt : public mosqpp::mosquittopp, public lidig_health
{
public:
    lidig_mqtt();
    lidig_mqtt(const std::string&, const std::string&, int, int);
    ~lidig_mqtt();

    void on_message(const struct mosquitto_message *message);
    void on_connect(int rc);
    void on_disconnect(int rc);
    int connect(const std::string& ip, int port);
    int start();
    void stop();

    lidig_mqtt& set_mqtt_port(const int port) {
        _mqtt_port = port;
        return *this;
    }
    lidig_mqtt& set_mqtt_keepalive(const int keepalive) {
        _mqtt_keepalive = keepalive;
        return *this;
    }
    lidig_mqtt& set_mqtt_host(const std::string& host) {
        _mqtt_host = host;
        return *this;
    }
    lidig_mqtt& set_will_topic(const std::string& will_topic) {
        _will_topic = will_topic;
        return *this;
    }
    void set_mqtt_user_passwd(const std::string& username, const std::string& password) {
        _username = username;
        _password = password;
    }
    const char* get_mqtt_host(void) {return _mqtt_host.c_str();}
    const char* get_mqtt_id(void) {return _mqtt_id.c_str();}
    int get_mqtt_port(void) {return _mqtt_port;}
    int get_mqtt_keepalive(void) {return _mqtt_keepalive;}
    void topic_sub_add(const std::string& topic) {
        topic_sub_.push_back(topic);
    }

    virtual void on_connection(lidig_mqtt* mqtt, int status) {}
    virtual void on_packet(lidig_mqtt* mqtt, const std::string& topic,
                const char* payload, size_t payloadlen) {}
    int mqtt_start();

protected:
    int health_cure();
    static void async_cb(lidig_async* async, void* arg);

private:
    std::string _mqtt_host;
    std::string _mqtt_id;
    std::string _will_topic;
    int _mqtt_port;
    int _mqtt_keepalive;
    std::string _username;
    std::string _password;
    std::vector<std::string> topic_sub_;

    typedef struct {
        std::string type;
        std::string topic;
        void *payload;
        int payloadlen;
        int rc;
        lidig_mqtt* self;

    } async_comm_t;

    lidig_queue_work work_;
};

#endif