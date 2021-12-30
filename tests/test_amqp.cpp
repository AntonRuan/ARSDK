#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_amqp.h"

using namespace std;

lidig_timer* timer_;
lidig_loop* loop_;
lidig_amqp* amqp_;

void time_cb(lidig_timer*) {
    static int flag = 0;
    if (flag == 0) {
        amqp_->publish("add", "第一个消息");
        flag = 1;
    } else {
        amqp_->publish("device.key", "the first message");
        flag = 0;
    }
}

int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;
    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    loop_ = new lidig_loop();

    timer_ = new lidig_timer();
    timer_->timer_start(2000, 1000, time_cb);

    amqp_ = new lidig_amqp();
    amqp_->connect("amqp://guest:guest@192.168.90.230/", "sdk");

    std::vector<string> v;
    v.push_back("add");
    v.push_back("delete");
    amqp_->bind_queue(v);

    amqp_->bind_queue("update");

    LogAlarm() << "start!";

    return loop_->run();
}