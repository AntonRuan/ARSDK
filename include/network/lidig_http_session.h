/*
 *  Created on: 2020/10/29
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_HTTP_SESSION_H__
#define __LIDIG_HTTP_SESSION_H__

#include <map>
#include <iostream>
#include <functional>
#include "util/http_parser.h"
#include "lidig_tcp_client.h"
#include "lidig_http_server.h"

class lidig_http_server;
class lidig_http_session : public lidig_tcp_client
{
public:
    lidig_http_session();
    ~lidig_http_session();

    void set_http_server(lidig_http_server* self);
    void add_header(const std::string &key, const std::string &value);
    void add_header(const std::string &key, const ssize_t value);
    void add_body(const char* data, ssize_t size);
    void add_body(const std::string &data);
    int send_data(const std::string &url);
    int send_response(const std::string &code, const std::string &status);
    std::string get_url() {return request_url_;};

    std::string schema();
    std::string host();
    std::string port();
    std::string path();
    std::string query();
    std::string fragment();
    std::string userInfo();

protected:
    virtual void on_read(lidig_stream* stream, const char* data, ssize_t nread) override;
    virtual void on_close(lidig_stream* stream) override;

    virtual void on_body(lidig_http_session* session, const char* data, ssize_t nread) {}
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {}

    std::string request_url_;
    std::string method_;

private:
    static int on_url(http_parser* parser, const char *at, size_t length);
    static int on_status(http_parser* parser, const char *at, size_t length);
    static int on_header_field(http_parser* parser, const char *at, size_t length);
    static int on_header_value(http_parser* parser, const char *at, size_t length);
    static int on_message_begin(http_parser* parser);
    static int on_headers_complete(http_parser* parser);
    static int on_message_complete(http_parser* parser);
    static int on_body(http_parser* parser, const char *at, size_t length);

    std::map<std::string,std::string> send_header_map_;
    std::map<std::string,std::string> recv_header_map_;
    std::string recv_body_str_;
    std::string send_body_str_;
    std::string field_;
    std::string response_url_;

    http_parser* parser_;
    http_parser_url* parser_url_;
    http_parser_settings http_settings_;

    lidig_http_server* http_server_ = nullptr;
};
#endif