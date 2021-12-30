#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_loop.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "base/lidig_thread.h"
#include "network/lidig_tcp_client.h"

using namespace std;
using namespace std::placeholders;

class test_tcp_client : public lidig_tcp_client
{
public:
    virtual void on_connection(lidig_tcp_client* client, int status) {
        LogInfo() << status;
    }

    virtual void on_write(lidig_stream* stream, const char* data, int status) {

    }

    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) {
        if (nread < 0)
            return;
        string str(data, nread);
        LogInfo() << str;

        stream->close();
    }

    virtual void on_close(lidig_stream* stream) {
        LogInfo();
    }
};

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

    lidig_loop loop;

    test_tcp_client* tcp_client_ = new test_tcp_client();
    tcp_client_->connect("192.168.31.73", 8000);

    lidig_timer timer;
    timer.timer_start(200, 1000,
        [&](lidig_timer*) {
            if (!tcp_client_)
                return;

            int ret = tcp_client_->async_write("123", 3);
            LOGI("tick ret = " << ret);
            timer.stop();
        }
    );

    LogAlarm() << "start!";

    return loop.run();
}