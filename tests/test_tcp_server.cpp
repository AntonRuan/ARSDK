#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_loop.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "base/lidig_thread.h"
#include "network/lidig_tcp_server.h"
#include "network/lidig_tcp_client.h"

using namespace std;

lidig_tcp_server* tcp_server_;
lidig_timer* lidig_timer_;
lidig_loop* loop_;


class test_tcp_client : public lidig_tcp_client
{
public:
    virtual void on_write(lidig_stream* stream, const char* data, int status) {

    }

    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) {
        if (nread < 0)
            return;

        std::string str((const char*)data, nread);
        LogInfo() << str;

        stream->async_write((const char*)data, nread);
    }

    virtual void on_close(lidig_stream* stream) {

    }

};
class test_tcp_server : public lidig_tcp_server
{
public:
    virtual void on_accept(lidig_tcp_client* client) {

    }

    virtual void on_connection(lidig_tcp_server* self, int status) {
        test_tcp_client* client = new test_tcp_client();
        if (accept(client) == 0)
            client->async_read_start();
        else
            delete client;
    }

    virtual void on_close(lidig_stream* stream) {

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

    loop_ = new lidig_loop();

    tcp_server_ = new test_tcp_server();
    tcp_server_->listen("0.0.0.0", 7000);

    LogAlarm() << "start!";

    return loop_->run();
}