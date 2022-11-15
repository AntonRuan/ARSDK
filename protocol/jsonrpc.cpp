#include "base/lidig_logger.h"
#include "protocol/jsonrpc.h"

using namespace std;

jrpc_base::jrpc_base() {
    LogTrace();
}

jrpc_base::jrpc_base(const std::string& data) {
    LogTrace();
    root_ = lidig_json::string_to_value(data);
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

jrpc_request::jrpc_request(const int id, const std::string method, const Json::Value params):
            jrpc_base(id), method_(method), params_(params) {
    root_[JRPC_NAME] = JRPC_NAME_VALUE;
    root_[JRPC_METHOD_NAME] = method;
    root_[JRPC_ID_NAME] = id;
    root_[JRPC_PARAMS_NAME] = params;
    id_ = id;
    method_ = method;
    params_ = params;
}


jrpc_request::jrpc_request(jrpc_base& jrpc): jrpc_base(jrpc) {
    if (root_.isMember(JRPC_METHOD_NAME))
        method_ = root_[JRPC_METHOD_NAME].asString();
    if (root_.isMember(JRPC_PARAMS_NAME))
        params_ = root_[JRPC_PARAMS_NAME];
}

jrpc_request::~jrpc_request() {

}

jrpc_response::jrpc_response(jrpc_request& req, const int result) {
    root_[JRPC_NAME] = JRPC_NAME_VALUE;
    root_[JRPC_ID_NAME] = req.id();
    root_[JRPC_RESULT_NAME] = result;
    id_ = req.id();
    result_ = result;
}

jrpc_response::jrpc_response(jrpc_base& jrpc): jrpc_base(jrpc) {
    if (root_.isMember(JRPC_RESULT_NAME))
        result_ = root_[JRPC_RESULT_NAME].asInt();
}

jrpc_response::~jrpc_response() {

}