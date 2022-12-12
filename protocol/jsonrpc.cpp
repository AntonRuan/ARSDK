#include "base/lidig_logger.h"
#include "protocol/jsonrpc.h"

using namespace std;


jrpc_base::jrpc_base() {
    LogTrace();
}

jrpc_base::jrpc_base(const std::string& data) {
    LogTrace();
    root_ = lidig_json::string_to_value(data);
    if (is_empty())
        return;
    if (root_.isMember(JRPC_ID_NAME))
        id_ = root_[JRPC_ID_NAME].asInt();
}

jrpc_base::jrpc_base(const char* data, const ssize_t size) {
    LogTrace();
    string str(data, size);
    root_ = lidig_json::string_to_value(str);
    if (is_empty())
        return;
    if (root_.isMember(JRPC_ID_NAME))
        id_ = root_[JRPC_ID_NAME].asInt();
}

jrpc_base::jrpc_base(const int id) {
    LogTrace();
    id_ = id;
}

jrpc_base::jrpc_base(jrpc_base& jrpc) {
    LogTrace();
    root_ = jrpc.get_root();
    if (root_.isMember(JRPC_ID_NAME))
        id_ = root_[JRPC_ID_NAME].asInt();
}

jrpc_base::~jrpc_base() {
    LogTrace();
}

bool jrpc_base::is_empty() {
    return root_.empty();
}

bool jrpc_base::is_request() {
    if (is_empty())
        return false;

    if (root_.isMember(JRPC_METHOD_NAME))
        return true;

    return false;
}

bool jrpc_base::is_response() {
    if (is_empty())
        return false;

    if (root_.isMember(JRPC_RESULT_NAME))
        return true;

    return false;
}

bool jrpc_base::is_error() {
    if (is_empty())
        return false;

    if (root_.isMember(JRPC_ERROR_NAME))
        return true;

    return false;
}

jrpc_request::jrpc_request(const int id, const std::string method, const Json::Value params):
            jrpc_base(id), method_(method), params_(params) {
    LogTrace();
    root_[JRPC_NAME] = JRPC_NAME_VALUE;
    root_[JRPC_METHOD_NAME] = method;
    root_[JRPC_ID_NAME] = id;
    root_[JRPC_PARAMS_NAME] = params;
    id_ = id;
    method_ = method;
    params_ = params;
}


jrpc_request::jrpc_request(jrpc_base& jrpc): jrpc_base(jrpc) {
    LogTrace();
    if (root_.isMember(JRPC_METHOD_NAME))
        method_ = root_[JRPC_METHOD_NAME].asString();
    if (root_.isMember(JRPC_PARAMS_NAME))
        params_ = root_[JRPC_PARAMS_NAME];
}

jrpc_request::~jrpc_request() {
    LogTrace();
}

jrpc_response::jrpc_response(jrpc_request& req, const Json::Value& result) {
    LogTrace();
    root_[JRPC_NAME] = JRPC_NAME_VALUE;
    root_[JRPC_ID_NAME] = req.id();
    root_[JRPC_RESULT_NAME] = result;
    id_ = req.id();
    result_ = result;
}

jrpc_response::jrpc_response(jrpc_base& jrpc): jrpc_base(jrpc) {
    LogTrace();
    if (root_.isMember(JRPC_RESULT_NAME))
        result_ = root_[JRPC_RESULT_NAME];
}

jrpc_response::~jrpc_response() {
    LogTrace();
}

jrpc_error::jrpc_error(jrpc_request& req, const int code) {
    LogTrace();
    error_map_[JRPC_PASER_ERROR] = JRPC_PASER_ERROR_MESSAGE;
    error_map_[JPRC_INVALID_REQUEST] = JPRC_INVALID_REQUEST_MESSAGE;
    error_map_[JPRC_METHOD_NOT_FOUND] = JPRC_METHOD_NOT_FOUND_MESSAGE;

    root_[JRPC_NAME] = JRPC_NAME_VALUE;
    root_[JRPC_ID_NAME] = req.id();
    Json::Value error;
    error[JRPC_CODE_NAME] = code;
    if (error_map_.find(code) != error_map_.end())
        error[JRPC_MESSAGE_NAME] = error_map_[code];
    else
        error[JRPC_MESSAGE_NAME] = JPRC_UNKNOWN_MESSAGE;
    root_[JRPC_ERROR_NAME] = error;
    id_ = req.id();
    code_ = code;
}

jrpc_error::jrpc_error(jrpc_base& jrpc): jrpc_base(jrpc) {
    LogTrace();
    error_map_[JRPC_PASER_ERROR] = JRPC_PASER_ERROR_MESSAGE;
    error_map_[JPRC_INVALID_REQUEST] = JPRC_INVALID_REQUEST_MESSAGE;
    error_map_[JPRC_METHOD_NOT_FOUND] = JPRC_METHOD_NOT_FOUND_MESSAGE;

    if (root_.isMember(JRPC_ERROR_NAME))
        code_ = root_[JRPC_ERROR_NAME][JRPC_CODE_NAME].asInt();
}

jrpc_error::~jrpc_error() {
    LogTrace();
}