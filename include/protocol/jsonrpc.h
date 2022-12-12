/*
 *  Created on: 2022/11/15
 *      Author: Anton Ruan
 */

#ifndef __jrpc_PROTOCOL__
#define __jrpc_PROTOCOL__

#include <string>
#include "util/lidig_json.h"

/*
--> {"josnrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
<-- {"josnrpc": "2.0", "result": 19, "id": 1}

<-- {"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"}
*/


#define JRPC_NAME                   "josnrpc"
#define JRPC_NAME_VALUE             "2.0"
#define JRPC_METHOD_NAME            "method"
#define JRPC_ID_NAME                "id"
#define JRPC_PARAMS_NAME            "params"
#define JRPC_RESULT_NAME            "result"
#define JRPC_ERROR_NAME             "error"
#define JRPC_CODE_NAME              "code"
#define JRPC_MESSAGE_NAME           "message"

#define JRPC_PASER_ERROR            -32700
#define JPRC_INVALID_REQUEST        -32600
#define JPRC_METHOD_NOT_FOUND       -32601

#define JRPC_PASER_ERROR_MESSAGE       "Parse error"
#define JPRC_INVALID_REQUEST_MESSAGE   "Invalid Request"
#define JPRC_METHOD_NOT_FOUND_MESSAGE  "Method not found"
#define JPRC_UNKNOWN_MESSAGE           "Unknown error"

typedef enum {
    JRPC_REQUEST        = 0,
    JRPC_RESPONSE,
    JRPC_NOTIFICATION,
    JRPC_ERROR,
    JRPC_WRONG_OBJECT
} jrpc_cmd_t;


class jrpc_base
{
public:
    jrpc_base();
    jrpc_base(jrpc_base& jrpc);
    jrpc_base(const int id);
    jrpc_base(const std::string& data);
    jrpc_base(const char* data, const ssize_t size);
    ~jrpc_base();

    int id() {
        return id_;
    }

    std::string to_string() {
        return lidig_json::value_to_string(root_);
    }

    bool is_empty();
    bool is_request();
    bool is_response();
    bool is_error();

    Json::Value get_root() {
        return root_;
    }

protected:
    int id_;

    Json::Value root_;
};

class jrpc_request: public jrpc_base
{
public:
    jrpc_request(jrpc_base& jrpc);
    jrpc_request(const int id, const std::string method, const Json::Value params);
    ~jrpc_request();

    std::string method() {
        return method_;
    }

    Json::Value params() {
        return params_;
    }
private:
    std::string method_;
    Json::Value params_;
};

class jrpc_response: public jrpc_base
{
public:
    jrpc_response(jrpc_base& jrpc);
    jrpc_response(jrpc_request& req, const Json::Value& result);
    ~jrpc_response();

    Json::Value result() {
        return result_;
    }
private:
    Json::Value result_;
};

class jrpc_error: public jrpc_base
{
public:
    jrpc_error(jrpc_base& jrpc);
    jrpc_error(jrpc_request& req, const int code);
    ~jrpc_error();

    int code() {
        return code_;
    }
private:
    int code_;
    std::map<int, std::string> error_map_;
};

#endif