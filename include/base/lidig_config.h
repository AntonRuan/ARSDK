/*
 *  Created on: 2020/2/8
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_CONFIG_H__
#define __LIDIG_CONFIG_H__

#include <vector>
#include "lidig_logger.h"
#include "util/lidig_uci.h"

class lidig_config
{
public:
    static std::string get_config(const std::string& path, const std::string& default_value);
    static int get_config(const std::string& path, int default_value);
    static bool set_config(const std::string& path, const std::string& value);
    static Json::Value load_config_to_json(const std::string& path, const std::string& type);
    static int save_json_to_config(const std::string& path, const std::string& type, Json::Value root);
};

template <class T>
class lidig_configs
{
public:
    static bool load_config(const std::string& path, const std::string& type, T& t) {
        Json::Value root;
        int ret = lidig_uci::load_section(path, type, root);
        if (!ret)
            return ret;

        std::vector<std::string> keys = root.getMemberNames();
        for (size_t j = 0; j < keys.size(); j++) {
            const std::string& cmd = keys[j];

            Json::Value& sub = root[cmd];
            for (auto& it: t.elements) {
                if (sub.isMember(it.first))
                    *it.second = sub[it.first].asString();
            }
            break;
        }

        return true;
    }

    static bool save_config(const std::string& path, const std::string& type, const T& t) {
        Json::Value root;

        for (auto& it: t.elements)
            root[it.first] = *it.second;

        std::string uuid = root["uuid"].asString();
        LogInfo() << uuid;
        if (!lidig_uci::is_have_section(path, uuid)) {
            LogInfo() << "add " << uuid;
            lidig_uci::add_section(path, type, uuid);
        }

        std::vector<std::string> keys = root.getMemberNames();
        for (size_t j = 0; j < keys.size(); j++) {
            const std::string& cmd = keys[j];
            lidig_uci::set_option_string(path, uuid, cmd, root[cmd].asString());
        }
        return true;
    }
};

#endif