#include "util/lidig_uci.h"
#include "base/lidig_config.h"
#include "base/lidig_logger.h"

using namespace std;


class device_info
{
public:
    device_info() {
        elements["uuid"] = &uuid;
        elements["status"] = &status;
        elements["type"] = &type;
        elements["model"] = &model;
        elements["manufacturer"] = &manufacturer;
        elements["version"] = &version;
    }

    std::string uuid;
    std::string status;
    std::string type;
    std::string model;
    std::string manufacturer;
    std::string version;

    std::map<std::string, std::string*> elements;
};



int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();


    if (lidig_uci::is_have_section("system", "lidig") == false) {
        LogInfo() << "add section";
        lidig_uci::add_section("system", "lidig", "lidig");
    }
    lidig_uci::set_option_string("system", "lidig", "hello", "world");
    vector<string> v;
    v.push_back("a");
    v.push_back("b");
    lidig_uci::set_option_list("system", "lidig", "nihao", v);

    string version = lidig_uci::get_option_string("system", "lidig", "hello");
    LogInfo() << version;


    device_info dev;
    lidig_configs<device_info>::load_config("subdevice", "device", dev);
    LogInfo() << dev.uuid;
    LogInfo() << dev.type;
}