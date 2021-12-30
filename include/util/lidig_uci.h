/*
 *  Created on: 2020/10/21
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_UCI_H__
#define __LIDIG_UCI_H__

#include <vector>
#include <string>
#include <uci.h>
#include "json/json.h"

class lidig_uci
{
public:
    lidig_uci();
    virtual ~lidig_uci();

    static std::string get_option_string(const std::string& path);
    static std::string get_option_string(const std::string& path, const std::string& defaultValue);
    static std::string get_option_string(const std::string& package, const std::string& section, const std::string& option);
    static std::string get_option_string(const std::string& package, const std::string& section, const std::string& option, const std::string& defaultValue);
    static std::string get_option_string(const std::string& package, const std::string& section, const std::string& option, int defaultValue);

    static int get_option_int(const std::string& package, const std::string& section, const std::string& option);
    static int get_option_int(const std::string& package, const std::string& section, const std::string& option, int defaultValue);
    static int get_option_int(const std::string& path, int defaultValue);

    static std::vector<std::string> get_option_list(const std::string& package, const std::string& section, const std::string& option);

    static bool set_option_string(const std::string& path, const std::string& value);
    static bool set_option_string(const std::string& package, const std::string& section, const std::string& option, const std::string& value);
    static bool set_option_int(const std::string& package, const std::string& section, const std::string& option, const int value);
    static bool set_option_list(const std::string& package, const std::string& section, const std::string& option, std::vector<std::string> v);

    static bool is_have_section(const std::string& package, const std::string& section);
    static bool add_section(const std::string& package, const std::string& section_type, const std::string& section_name);
    static std::vector<std::string> get_section(const std::string& package, const std::string& section_type);

    /*static bool set_option_string(struct uci_context *ctx, const std::string& package, const std::string& section, const std::string& option, const std::string& value);
    static bool set_option_string(struct uci_context *ctx, const std::string& package, const std::string& section, const std::string& option, std::string value);
    static bool set_option_int(struct uci_context *ctx, const std::string& package, const std::string& section, const std::string& option, int value);
    static bool add_section(const std::string& package, const std::string& section, Json::Value root);*/
    static Json::Value load_section(const std::string& package, const std::string& section_type);
    static bool load_section(const std::string& package, const std::string& section_type, Json::Value& root);
};

#endif