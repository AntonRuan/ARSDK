#include <stdio.h>
#include <stdlib.h>
#include "base/lidig_cmd.h"
#include "protocol/zabbix.h"
#include "base/lidig_file.h"
#include "base/lidig_logger.h"
#include "base/lidig_fs_event.h"
#include "network/lidig_tcp_client.h"

using namespace std;

class sync_file : public lidig_fs_event, public lidig_tcp_client, public lidig_file, public zabbix_protocol
{
public:
    sync_file() {}
    ~sync_file() {}

    virtual void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        string filename = file->get_filename();
        string send_data = encode_packet(data, nread, filename.c_str(), filename.size());
        lidig_tcp_client::async_write(send_data);
    }

    virtual void on_changed(lidig_fs_event* file, const char* filename, int events) override {
        if (filename[0] == '.')
            return;

        if (events & UV_RENAME) {
            LogInfo() << "Change detected: " << "renamed " << filename;
            readfile(filename);
        }

        if (events & UV_CHANGE) {
            LogInfo() << "Change detected: " << "changed " << filename;
            readfile(filename);
        }
    }

    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        string str(data, nread);
        LogInfo() << str;
    }
};

int main(int argc, const char **argv)
{
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

    sync_file sync;
    sync.init(".");
    sync.connect("172.16.246.15", 8000);

    LogAlarm() << "start!";

    return loop.run();
}