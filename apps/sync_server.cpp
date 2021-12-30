#include <stdio.h>
#include <stdlib.h>
#include "base/lidig_cmd.h"
#include "protocol/zabbix.h"
#include "base/lidig_file.h"
#include "base/lidig_logger.h"
#include "base/lidig_fs_event.h"
#include "network/lidig_tcp_client.h"
#include "network/lidig_tcp_server.h"

using namespace std;

class sync_file_client: public lidig_tcp_client, public lidig_file, public zabbix_protocol
{
public:
    virtual void on_packet(const std::string& data, const std::string& reserved) override {
        open(reserved, O_CREAT | O_TRUNC);
        lidig_file::async_write(data.c_str(), data.size(), 0);
    }

    using lidig_file::on_read;
    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) override {
        parser_packet(data, nread);
    }

    using lidig_tcp_client::on_write;
    virtual void on_write(lidig_file* file, const char* data) override {
        lidig_file::close();
    }
};

class sync_file_server: public lidig_tcp_server
{
public:
    sync_file_server() {}
    ~sync_file_server() {}

    virtual void on_connection(lidig_tcp_server* self, int status) override {
        sync_file_client* client = new sync_file_client();
        if (accept(client) == 0)
            client->async_read_start();
        else
            delete client;
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

    sync_file_server sync;
    sync.listen("0.0.0.0", 8000);


    LogAlarm() << "start!";

    return loop.run();
}