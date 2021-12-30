#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

using namespace std;

lidig_timer* timer_;
lidig_timer* timer1_;

void time_cb(lidig_timer* self) {
    LogInfo();
}

void time1_cb(lidig_timer* self) {
    static bool flag = false;
    if (flag) {
        LogInfo() << "stop";
        timer_->stop();
        flag = false;
    } else {
        LogInfo() << "start";
        timer_->start();
    }
}

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    timer_ = new lidig_timer();
    timer_->timer_start(2000, 2000, time_cb);

    timer1_ = new lidig_timer();
    timer1_->timer_start(1000, 1000, time1_cb);

    return loop.run();
}