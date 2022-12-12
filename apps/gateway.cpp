#include "base/lidig_cmd.h"
#include "util/lidig_crc.h"
#include "util/lidig_json.h"
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_mqtt.h"

using namespace std;

class test_mqtt : public lidig_mqtt, public lidig_timer, public lidig_file
{
public:
    void on_connection(lidig_mqtt* para, int status) override {
        LogInfo() << status;
    }

    void on_packet(lidig_mqtt* mqtt, const std::string& key, const char* data, size_t len) override {
        //LogInfo() << key << " " << string(data, len);
    }

    void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        close();

        string str(data, nread-1);
        vector<string> s = lidig_crc::split(str, " ");
        if (s.size() == 0)
            return;

        switch(cpu_) {
        case CPULOAD:
            LogInfo() << "temp:" << stoi(s[0])/1000;
            temp_ = s[0].substr(0, s[0].size()-3);
            cpu_ = TEMPERATURE;
            break;
        case TEMPERATURE:
            LogInfo() << "loadavg:" << stof(s[0]);
            load_ = s[0];
            cpu_ = RES_MAX;
            break;
        default:
            break;
        }

        Json::Value root, info, cpu;

        cpu["load_5min_prcnt"] = load_;
        info["cpu"] = cpu;
        info["temperature_c"] = temp_;
        root["info"] = info;

        string send_data = lidig_json::value_to_string(root);
        publish(nullptr, "home/nodes/sensor/rpi-nas/monitor", send_data.size(), send_data.data(), 2);
    }

    void on_time(lidig_timer*) override {
        if (get_fd() != 0)
            return;

        switch(cpu_) {
        case CPULOAD:
            open("/sys/class/thermal/thermal_zone0/temp", 0);
            async_read(30, 0);
            break;
        case TEMPERATURE:
            open("/proc/loadavg", 0);
            async_read(30, 0);
            break;
        default:
            cpu_ = CPULOAD;
            break;
        }
    }

private:
    typedef enum resource {
        CPULOAD,
        TEMPERATURE,
        RES_MAX
    } cpu_t;
    cpu_t cpu_;

    string load_;
    string temp_;
};

#if 0
static int luv_cpu_info() {
    uv_cpu_info_t* cpu_infos;
    int count, i;
    int ret = uv_cpu_info(&cpu_infos, &count);
    if (ret < 0)
        return -1;

    for (i = 0; i < count; i++) {
        LogInfo() << "model " << cpu_infos[i].model;
        LogInfo() << "speed " << cpu_infos[i].speed;
        LogInfo() << "user " << cpu_infos[i].cpu_times.user;
        LogInfo() << "nice " << cpu_infos[i].cpu_times.nice;
        LogInfo() << "sys " << cpu_infos[i].cpu_times.sys;
        LogInfo() << "idle " << cpu_infos[i].cpu_times.idle;
        LogInfo() << "irq " << cpu_infos[i].cpu_times.irq;
    }

    uv_free_cpu_info(cpu_infos, count);
    return 1;
}
#endif

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "gateway", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    test_mqtt mqtt;
    mqtt.connect("zhang.codec.wang", 1883);
    mqtt.topic_sub_add("stat/switch/POWER");
    mqtt.topic_sub_add("stat/switch/RESULT");
    mqtt.topic_sub_add("stat/light/POWER");
    mqtt.topic_sub_add("stat/light/RESULT");
    //mqtt.topic_sub_add("home/nodes/sensor/rpi-nas/monitor");
    mqtt.lidig_mqtt::start();
    mqtt.timer_start(200, 3000);

    LogInfo() << "start!";

    return loop.run();
}
