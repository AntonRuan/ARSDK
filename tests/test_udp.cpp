#include <string.h>
#include "base/lidig_timer.h"
#include "network/lidig_udp.h"

using namespace std;

lidig_udp* udp_;
lidig_timer* lidig_timer_;
lidig_loop* loop_;

void on_read(const char* data, ssize_t nread, std::string ip, int port)
{
    printf("%s\n", (char*)data);
    udp_->async_write(ip, port, data, nread);
}

void time_cb(lidig_timer*)
{
    string str = "hello world!\n";
    int ret = udp_->async_write("192.168.31.208", 5002, str.c_str(), str.size());
    printf("tick %d\n", ret);
    //lidig_timer_->stop();
}

int main(int argc, char const *argv[])
{
    loop_ = new lidig_loop();

    udp_ = new lidig_udp(loop_);

    //udp_->bind("0.0.0.0", 7000);
    udp_->async_read_start(on_read);

    lidig_timer_ = new lidig_timer();
    lidig_timer_->timer_start(0, 1000, time_cb);

    return loop_->run();
}