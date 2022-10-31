#include <stdio.h>
#include <stdlib.h>
#include "base/lidig_file.h"
#include "base/lidig_logger.h"
#include "base/lidig_fs_event.h"
#include "network/lidig_tcp_client.h"

using namespace std;


class sync_file : public lidig_fs_event, public lidig_tcp_client, public lidig_file
{
public:
    sync_file() {}
    ~sync_file() {}

    virtual void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        lidig_tcp_client::async_write(file->get_filename());
        lidig_tcp_client::async_write(data, nread);
    }

    virtual void on_changed(lidig_fs_event* file, const char* filename, int events) override {
        if (filename[0] == '.')
            return;

        if (events & UV_RENAME) {
            LogInfo() << "Change detected: " << "renamed " << filename;
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

int main(int argc, char **argv)
{
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    sync_file sync;
    sync.init(".");
    sync.connect("172.16.246.15", 8000);


    LogAlarm() << "start!";

    return loop.run();
}