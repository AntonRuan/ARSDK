/*
 *  Created on: 2020/11/5
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_HEALTH_H__
#define __LIDIG_HEALTH_H__

#include <vector>
#include "lidig_loop.h"
#include "lidig_timer.h"

class lidig_health
{
public:
    enum HEALTH_STATUS {
        HEALTH_NOT_CHECK,   // not start
        HEALTH_CHECK,       // suspend(只有在good之后才可以设置)
        HEALTH_SICK,        // something err
        HEALTH_GOOD,        // normal work
    };

    lidig_health(const std::string& name = "default");
    ~lidig_health();

    void set_check();
    void set_sick(const std::string& error = "error");
    void set_good();

    bool is_sick() {return (status_ == HEALTH_SICK);};
    bool is_good() {return (status_ == HEALTH_GOOD);};

    std::string get_name() {return name_;};
    void set_alert(const std::string& alert);
    void send_alert(const std::string& name, const std::string& type, const std::string& message);

    virtual int health_cure(void) = 0;
    int err_count_;
    int err_double_;

private:
    HEALTH_STATUS status_;
    std::string name_;
};

typedef std::function<void(std::string,std::string,std::string)> send_alert_callback;
class health_server
{
public:
    health_server(uint64_t timeout = 0, uint64_t repeat = 5000);
    ~health_server();

    void run(void);
    void add_obj(lidig_health* pobj);
    void erase_obj(lidig_health* pobj);
    void send_alert(const std::string& name, const std::string& type, const std::string& message);
    void set_send_alert_cb(send_alert_callback cb) {send_alert_cb_ = cb;};
    send_alert_callback send_alert_cb_ = nullptr;

private:
    lidig_timer* timer_;
    std::vector<lidig_health*> obj_;
};

extern void default_health_init();
extern void default_health_init_cb(send_alert_callback cb);

#endif