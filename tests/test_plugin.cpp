#include <uv.h>
#include "base/lidig_timer.h"
#include "base/lidig_loop.h"
#include "base/lidig_plugin.h"

using namespace std;

lidig_timer* lidig_timer_;
lidig_loop* loop_;


void time_cb(lidig_timer*)
{
    lidig_timer_->stop();
}

void on_read(const char* data, ssize_t size)
{
    printf("on_read %s %ld\n", data, size);
}

int main(int argc, char const *argv[])
{
    loop_ = new lidig_loop();

    lidig_timer_ = new lidig_timer();
    lidig_timer_->timer_start(0, 1000, time_cb);


    lidig_plugin dev;
    dev.init("test/ubus.so");
    dev.open();
    dev.read_start(on_read);
    dev.write("123", 4);
    dev.close();

    return loop_->run();
}