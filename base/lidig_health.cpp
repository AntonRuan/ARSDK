#include "base/lidig_health.h"
#include "base/lidig_logger.h"

health_server* default_health;
void default_health_init() {
    default_health = new health_server(5000, 5000);
}

void default_health_init_cb(send_alert_callback cb) {
    if (default_health)
        default_health->set_send_alert_cb(cb);
}

lidig_health::lidig_health(const std::string& name)
            : status_(HEALTH_NOT_CHECK), name_(name) {
    LogTrace();

    err_count_ = 0;
    err_double_ = 1;
    if (default_health)
        default_health->add_obj(this);
}

lidig_health::~lidig_health() {
    if (default_health)
        default_health->erase_obj(this);

    LogTrace();
}

void lidig_health::set_check() {
    if (status_ == HEALTH_CHECK)
        return;
    status_ = HEALTH_CHECK;
}

void lidig_health::set_sick(const std::string& error) {
    if (status_ == HEALTH_SICK)
        return;
    status_ = HEALTH_SICK;
    if (default_health)
        default_health->send_alert(name_, "error", error);
}

void lidig_health::set_good() {
    if (status_ == HEALTH_GOOD)
        return;
    status_ = HEALTH_GOOD;
    err_count_ = 0;
    err_double_ = 1;
    if (default_health)
        default_health->send_alert(name_, "good", "Good");
}

void lidig_health::set_alert(const std::string& alert) {
    if (default_health)
        default_health->send_alert(name_, "alert", alert);
}

void lidig_health::send_alert(const std::string& name,
                    const std::string& type, const std::string& message) {
    if (default_health)
        default_health->send_alert(name, type, message);
}

health_server::health_server(uint64_t timeout, uint64_t repeat) {
    LogTrace();

    timer_start(timeout, repeat);
}

health_server::~health_server() {
    LogTrace();
}

void health_server::on_time(lidig_timer* timer) {
    run();
}

void health_server::run(void) {
    for (auto it = obj_.begin(); it != obj_.end(); it++) {
        lidig_health* h = (lidig_health*) *it;
        if (h->is_sick()) {
            h->err_count_++;
            if (h->err_count_ == h->err_double_) {
                h->err_double_ *= 2;
                if (h->health_cure() == 0)
                    h->set_check();
            } else if (h->err_count_ > 1000) {
                h->err_count_ = 0;
                h->err_double_ = 1;
            }
        }
    }
}

void health_server::add_obj(lidig_health* pobj) {
    for (auto it = obj_.begin(); it != obj_.end(); it++) {
        if ((*it) == pobj)
            return;
    }
    obj_.push_back(pobj);
}

void health_server::erase_obj(lidig_health* pobj) {
    for (auto it = obj_.begin(); it != obj_.end(); it++) {
        if ((*it) == pobj) {
            obj_.erase(it);
            break;
        }
    }
}

void health_server::send_alert(const std::string& name,
            const std::string& type, const std::string& message) {
    if (send_alert_cb_)
        send_alert_cb_(name, type, message);
}