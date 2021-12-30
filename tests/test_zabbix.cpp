#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

using namespace std;

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    /*lidig_zabbix zabbix(&loop);
    zabbix.set_on_packet_cb(
        [](lidig_zabbix* self, Json::Value root, const char* data, ssize_t nread) {
            if (nread < 0)
                return;
            LogInfo() << root["response"].asString();
        }
    );
    zabbix.connect("192.168.90.224", 10051,
        [&](lidig_tcp_client* self, int status) {
            if (status < 0)
                return;
            zabbix.add_data("ruan", "agent.version", "1.0.0");
            zabbix.add_data("ruan", "agent.hostname", 20);
            zabbix.send_data();
        }
    );


    lidig_zabbix zabbix1(&loop);
    zabbix1.add_data("ruan", "agent.version", "1.0.2");
    zabbix1.add_data("ruan", "agent.hostname", 12);
    zabbix1.send_request("192.168.90.224", 10051,
        [](lidig_zabbix* self, Json::Value root, const char* data, ssize_t nread) {
            if (nread < 0)
                return;
            LogInfo() << root["response"].asString();
        }
    );*/

    LogInfo() << "start!";

    return loop.run();
}