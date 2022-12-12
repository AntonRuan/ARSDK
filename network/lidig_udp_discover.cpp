#include <string.h>
#include "base/lidig_logger.h"
#include "network/lidig_udp_discover.h"

using namespace std;

lidig_udp_discover::lidig_udp_discover() {
    LogTrace();
}

lidig_udp_discover::~lidig_udp_discover() {
    LogTrace();
}

void lidig_udp_discover::on_read(lidig_udp* udp, const char* data, ssize_t nread, std::string ip, int port) {
    if (ip == local_ip_)
        return;

    LogDebug() << "from " << ip << ":" << port;
    if (ip_map_.find(ip) == ip_map_.end()) {
        ip_info_t info;
        info.port = port;
        info.alive_count = 0;
        ip_map_[ip] = info;
        on_new_ip(ip);
    } else {
        ip_map_[ip].alive_count = 0;
        ip_map_[ip].port = port;
    }
}

void lidig_udp_discover::on_time(lidig_timer* timer) {
    async_write("255.255.255.255", 9999, " ");
    for (auto it = ip_map_.begin(); it != ip_map_.end();) {
        it->second.alive_count++;
        if (it->second.alive_count > 10) {
            on_delete_ip(it->first);
            ip_map_.erase(it++);
        } else {
            it++;
        }
    }
}

int lidig_udp_discover::async_read_start() {
    local_ip_ = get_local_ip();
    set_broadcast();
    timer_start(1000, 5000);
    return lidig_udp::async_read_start();
}