#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include "util/lidig_uci.h"
#include "base/lidig_logger.h"

using namespace std;

lidig_uci::lidig_uci() {
}

lidig_uci::~lidig_uci() {
}

string lidig_uci::get_option_string(const string& path) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;
    string ret;

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        //LogDebug() << path.c_str() << " null";
        uci_free_context(ctx);
        return ret;
    }

    if (p.o == NULL) {
        LogTrace() << path << " empty";
        uci_free_context(ctx);
        return ret;
    }
    ret = p.o->v.string;
    //LogTrace() << path << " get " << ret;
    uci_free_context(ctx);
    return ret;
}

string lidig_uci::get_option_string(const string& package, const string& section, const string& option) {
    string path = package + "." + section + "." + option;
    return get_option_string(path);
}

string lidig_uci::get_option_string(const string& path, const string& defaultValue) {
    string ret = get_option_string(path);
    if (ret.empty()) {
        ret = defaultValue;
    }
    return ret;
}

string lidig_uci::get_option_string(const string& package, const string& section, const string& option, int defaultValue) {
    string ret = get_option_string(package, section, option);
    if (ret.empty()) {
        stringstream ss;
        ss << defaultValue;
        ss >> ret;
    }
    return ret;
}

string lidig_uci::get_option_string(const string& package, const string& section, const string& option, const string& defaultValue) {
    string ret = get_option_string(package.c_str(), section.c_str(), option.c_str());
    if (ret.empty()) {
        ret = defaultValue;
    }
    return ret;
}

int lidig_uci::get_option_int(const string& package, const string& section, const string& option) {
    string ret = get_option_string(package, section, option);
    int r = 0;
    if (!ret.empty()) {
        r = stoi(ret); //有安全问题
    }
    return r;
}

int lidig_uci::get_option_int(const string& package, const string& section, const string& option, int defaultValue) {
    string ret = get_option_string(package, section, option);
    int r = defaultValue;
    if (!ret.empty()) {
        r = stoi(ret); //有安全问题
    }
    return r;
}

int lidig_uci::get_option_int(const string& path, int defaultValue) {
    string ret = get_option_string(path);
    int r = defaultValue;
    if (!ret.empty()) {
        r = stoi(ret); //有安全问题
    }
    return r;
}


bool lidig_uci::set_option_string(const string& path, const string& value) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        LogError() << path;
        uci_free_context(ctx);
        return false;
    }

    p.value = value.c_str();
    uci_set(ctx, &p);
    uci_commit(ctx, &p.p, false);
    uci_free_context(ctx);
    return true;
}

bool lidig_uci::set_option_string(const string& package, const string& section, const string& option, const string& value) {
    string path = package + "." + section + "." + option;
    return set_option_string(path, value);
}

bool lidig_uci::set_option_int(const string& package, const string& section, const string& option, const int value) {
    return set_option_string(package, section, option, to_string(value));
}


std::vector<std::string> lidig_uci::get_option_list(const string& package, const string& section, const string& option) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;
    std::vector<std::string> ret;
    string path = package + "." + section + "." + option;
    ret.clear();

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        LogDebug() << path;
        uci_free_context(ctx);
        return ret;
    }

    if (p.o == NULL) {
        uci_free_context(ctx);
        return ret;
    }

    uci_element *ele;
    uci_foreach_element(&p.o->v.list, ele) {
        ret.push_back(string(ele->name));
    }
    uci_free_context(ctx);
    return ret;
}

bool lidig_uci::set_option_list(const string& package, const string& section, const string& option, vector<string> v) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;
    string ret;
    string path = package + "." + section + "." + option;

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        LogError() << path;
        uci_free_context(ctx);
        return false;
    }

    uci_delete(ctx, &p);
    for (auto& it : v) {
        p.value = it.c_str();
        uci_add_list(ctx, &p);
    }

    uci_commit(ctx, &p.p, false);
    uci_free_context(ctx);
    return true;
}

bool lidig_uci::add_section(const string& package, const string& section_type, const string& section_name) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;
    string ret;
    string path = package + "." + section_name;

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        LogError() << path;
        uci_free_context(ctx);
        return false;
    }

    p.value = section_type.c_str();
    uci_set(ctx, &p);
    uci_commit(ctx, &p.p, false);
    uci_free_context(ctx);
    return true;
}

bool lidig_uci::is_have_section(const string& package, const string& section) {
    struct uci_context* ctx = uci_alloc_context();
    struct uci_ptr p;
    string ret;
    string path = package + "." + section;

    if (UCI_OK != uci_lookup_ptr(ctx, &p, (char*)path.c_str(), true)) {
        LogError() << path;
        uci_free_context(ctx);
        return false;
    }

    if (p.s == NULL) {
        uci_free_context(ctx);
        return false;
    }
    uci_free_context(ctx);
    return true;
}

vector<string> lidig_uci::get_section(const string& package, const string& section_type) {
    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *pkg = NULL;
    struct uci_element *ele;
    vector<string> v;

    if (UCI_OK != uci_load(ctx, package.c_str(), &pkg))  {
        uci_free_context(ctx);
        return v;
    }

    uci_foreach_element(&pkg->sections, ele) {
        struct uci_section *s = uci_to_section(ele);
        if (section_type != s->type)
            continue;

        v.push_back(s->e.name);
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return v;
}

/*bool lidig_uci::set_option_string(struct uci_context *ctx, const string& package, const string& section, const string& option, const string& value) {
    struct uci_ptr ptr;
    char path[255];
    sprintf(path, "%s.%s.%s", package, section, option);

    if (UCI_OK != uci_lookup_ptr(ctx, &ptr, path, true)) {
        LogError() << package << "." << section << "." << option;
        uci_free_context(ctx);
        return false;
    }

    ptr.value = value;
    uci_set(ctx, &ptr);
    return true;
}

bool lidig_uci::set_option_string(struct uci_context *ctx, const string& package, const string& section, const string& option, string value) {
    return set_option_string(ctx, package, section, option, value.c_str());
}

bool lidig_uci::set_option_int(struct uci_context *ctx, const string& package, const string& section, const string& option, int value) {
    return set_option_string(ctx, package, section, option, to_string(value));
}

bool lidig_uci::add_section(const string& package, const string& section, Json::Value root) {
    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *pkg = NULL;
    struct uci_section *s = NULL;

    if (UCI_OK != uci_load(ctx, package, &pkg)) {
        uci_free_context(ctx);
        return false;
    }

    if ((pkg = uci_lookup_package(ctx, package)) == NULL) {
        uci_unload(ctx, pkg);
        uci_free_context(ctx);
        return false;
    }

    uci_add_section(ctx, pkg, section, &s);

    vector<string> keys = root.getMemberNames();
    for (size_t j = 0; j < keys.size(); j++) {
        const string& cmd = keys[j];
        Json::Value value = root[cmd];
        if (value.isInt())
            set_option_int(ctx, package, s->e.name, cmd.c_str(), value.asInt());
        else if (value.isString())
            set_option_string(ctx, package, s->e.name, cmd.c_str(), value.asString());
    }

    uci_commit(ctx, &pkg, false);
    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return true;
}

*/
Json::Value lidig_uci::load_section(const string& package, const string& section_type) {
    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *pkg = NULL;
    struct uci_element *ele;
    Json::Value root;

    if (UCI_OK != uci_load(ctx, package.c_str(), &pkg)) {
        uci_free_context(ctx);
        return root;
    }

    uci_foreach_element(&pkg->sections, ele) {
        struct uci_section *s = uci_to_section(ele);
        LOGFMTT("section type is %s, name is %s", s->type, s->e.name);
        if (section_type != s->type)
            continue;

        Json::Value array;
        struct uci_element *e;
        uci_foreach_element(&s->options, e) {
            struct uci_option *o = uci_to_option(e);
            if (o->type != UCI_TYPE_STRING)
                return root;

            LOGFMTT("option name is %s, value is %s.", o->e.name, o->v.string);
            array[o->e.name] = o->v.string;
        }
        root[s->e.name] = array;
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return root;
}

bool lidig_uci::load_section(const string& package, const string& section_type, Json::Value& root) {
    struct uci_context *ctx = uci_alloc_context();
    struct uci_package *pkg = NULL;
    struct uci_element *ele;

    if (UCI_OK != uci_load(ctx, package.c_str(), &pkg)) {
        uci_free_context(ctx);
        return false;
    }

    uci_foreach_element(&pkg->sections, ele) {
        struct uci_section *s = uci_to_section(ele);
        LOGFMTT("section type is %s, name is %s", s->type, s->e.name);
        if (section_type != s->type)
            continue;

        Json::Value array;
        struct uci_element *e;
        uci_foreach_element(&s->options, e) {
            struct uci_option *o = uci_to_option(e);
            if (o->type != UCI_TYPE_STRING)
                continue;

            LOGFMTT("option name is %s, value is %s.", o->e.name, o->v.string);
            array[o->e.name] = o->v.string;
        }
        root[s->e.name] = array;
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);
    return true;
}
