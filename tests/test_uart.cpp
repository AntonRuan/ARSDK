#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_uart.h"

using namespace std;


lidig_timer* timer_;
lidig_loop* loop_;
lidig_uart* dev_;

class test_uart : public lidig_uart
{
public:
    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) {
        string str((char*)data, nread);
        LogInfo() << "[" << nread << "] " << str;
        timer_->stop();
        dev_->close();
    }

    virtual void on_close(lidig_stream* stream) {
        //delete dev_;
        delete timer_;
    }
};

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_TRACE;
    if (cmd.find('c') == cmd.end())
        return -1;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    loop_ = new lidig_loop();

    dev_ = new test_uart();
    int ret = dev_->open(cmd['c'], 115200);
    LogInfo() << ret;

    timer_ = new lidig_timer();
    timer_->timer_start(100, 5000,
        [&](lidig_timer*) {
            string AT = cmd['s'] + "\r\n";
            dev_->async_write(AT);
        }
    );

    return loop_->run();
}