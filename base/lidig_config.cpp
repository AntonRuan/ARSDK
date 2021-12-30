#include "base/lidig_config.h"

using namespace std;

string lidig_config::get_config(const string& path, const string& default_value) {
    return lidig_uci::get_option_string(path, default_value);
}

int lidig_config::get_config(const string& path, int default_value) {
    return lidig_uci::get_option_int(path, default_value);
}

bool lidig_config::set_config(const string& path, const string& value) {
    return lidig_uci::set_option_string(path, value);
}

Json::Value lidig_config::load_config_to_json(const string& path, const string& type) {
    return lidig_uci::load_section(path, type);
}

int lidig_config::save_json_to_config(const string& path, const string& type, Json::Value root) {
	if (!root.isMember("uuid"))
        return -1;

    string uuid = root["uuid"].asString();
    LogInfo() << uuid;
    if (!lidig_uci::is_have_section(path, uuid)) {
        LogInfo() << "add";
        lidig_uci::add_section(path, type, uuid);
    }

    vector<string> keys = root.getMemberNames();
    for (size_t j = 0; j < keys.size(); j++) {
        const string& cmd = keys[j];
        lidig_uci::set_option_string(path, uuid, cmd, root[cmd].asString());
    }
    return 0;
}