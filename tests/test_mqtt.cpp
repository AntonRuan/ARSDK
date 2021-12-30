#include "base/lidig_cmd.h"
#include "protocol/ldbd.h"
#include "util/lidig_json.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_mqtt.h"

using namespace std;

class test_mqtt : public lidig_mqtt
{
public:
    void on_connection(lidig_mqtt* para, int status) override {
        LogInfo();
    }

    void on_packet(lidig_mqtt* mqtt, const std::string&, const char*, size_t) override {
        string context = "response";
        string data = ldbd_protocol::encode_packet(context, ldbd_format_json_respond, 1);
        mqtt->publish(nullptr, "/R/LDSC04/L123456", data.size(), data.data(), 2);
    }
};

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

    test_mqtt mqtt;
    mqtt.connect("192.168.31.1", 1883);
    mqtt.topic_sub_add("/C/LDSC04/L123456");
    mqtt.topic_sub_add("/F/LDSC04/L123456");
    mqtt.set_will_topic("/W/LDSC04/L123456");
    mqtt.start();

    lidig_timer timer;
    timer.timer_start(200, 3000,
        [&](lidig_timer*) {
            static int flag = 0;
            if (flag == 0) {
                Json::Value root;
                root["mac_v4"] = "123";
                root["ip"] = "192.168.90.43";
                root["netmask"] = "255.255.255.0";
                root["gateway"] = "192.168.90.1";
                root["dns"] = "192.168.90.1";
                root["port"] = 0;
                root["remote_ip"] = "192.168.90.207";
                root["remote_port"] = 1883;

                string context = lidig_json::value_to_string(root);
                string data = ldbd_protocol::encode_packet(context, ldbd_format_json_heartbeat, 1);
                mqtt.publish(nullptr, "/U/LDSC04/L123456", data.size(), data.data(), 2);

                flag = 1;
            } else {
                //mqtt.stop();
                timer.stop();
            }
        }
    );

    LogInfo() << "start!";

    return loop.run();
}

//string messgage = "{\"ac_frequency\":4999,\"ac_voltage\":2261,\"ac_voltage_status\":1,\"cameras\":[{\"current\":0,\"index\":0,\"power\":2,\"powerFactor\":0,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":1,\"power\":2,\"powerFactor\":0,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":2,\"power\":2,\"powerFactor\":0,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":3,\"power\":2,\"powerFactor\":0,\"quantity\":0,\"status\":1}],\"dc12v_status\":1,\"dc12v_voltage\":123,\"dcups_status\":1,\"dcups_voltage\":123,\"delay_close_time\":\"0\",\"delay_open_time\":\"0\",\"device_time\":\"2020-12-07 16:12:10\",\"dns\":\"114.114.114.114\",\"door_status\":0,\"filllights\":[{\"current\":0,\"index\":0,\"power\":2,\"powerFactor\":0,\"pwm\":100,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":1,\"power\":2,\"powerFactor\":0,\"pwm\":15,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":2,\"power\":2,\"powerFactor\":0,\"pwm\":100,\"quantity\":0,\"status\":1},{\"current\":0,\"index\":3,\"power\":2,\"powerFactor\":0,\"pwm\":28,\"quantity\":0,\"status\":1}],\"firmware_version\":\"11112\",\"fixed_close_time\":\"23:00:00\",\"fixed_open_time\":\"00:10:00\",\"gateway\":\"192.168.90.1\",\"gateway_current\":0,\"gateway_version\":\"2.0.1\",\"ip\":\"192.168.90.101\",\"light_model\":1,\"mac_v4\":\"78:a3:51:46:4b:46\",\"modem_current\":1490,\"netmask\":\"255.255.255.0\",\"port\":0,\"remote_ip\":\"192.168.90.207\",\"remote_port\":1883,\"temperature\":246}";
