#include "base/lidig_logger.h"
#include "network/lidig_http_session.h"

using namespace std;

lidig_http_session::lidig_http_session() {
    LogTrace();

    parser_ = (http_parser *)malloc(sizeof(http_parser));
    if (!parser_) LogError() << "http_parser malloc failed!";
    parser_url_ = (http_parser_url*)malloc(sizeof(http_parser_url));
    if (!parser_url_) LogError() << "http_parser_url malloc failed!";
    parser_->data = this;

    http_settings_.on_message_begin = &lidig_http_session::on_message_begin;
    http_settings_.on_url = &lidig_http_session::on_url;
    http_settings_.on_status = &lidig_http_session::on_status;
    http_settings_.on_header_field = &lidig_http_session::on_header_field;
    http_settings_.on_header_value = &lidig_http_session::on_header_value;
    http_settings_.on_headers_complete = &lidig_http_session::on_headers_complete;
    http_settings_.on_body = &lidig_http_session::on_body;
    http_settings_.on_message_complete = &lidig_http_session::on_message_complete;
    http_settings_.on_chunk_header = nullptr;
    http_settings_.on_chunk_complete = nullptr;

    http_parser_init(parser_, HTTP_BOTH);
    http_parser_url_init(parser_url_);
}

lidig_http_session::~lidig_http_session() {
    if (parser_) free(parser_);
    if (parser_url_) free(parser_url_);

    LogTrace();
}

int lidig_http_session::on_body(http_parser* parser, const char* at, size_t length) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    self->recv_body_str_.append(at, length);
    self->on_body(self, at, length);
    //printf("Body: %.*s\n", (int)length, at);
    return 0;
}

int lidig_http_session::on_header_field(http_parser* parser, const char *at, size_t length) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    self->field_ = string(at, length);
    return 0;
}

int lidig_http_session::on_header_value(http_parser* parser, const char *at, size_t length) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    self->recv_header_map_[self->field_] = string(at, length);
    return 0;
}

int lidig_http_session::on_message_begin(http_parser* parser) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);

    LOGD("***MESSAGE BEGIN***");
    self->recv_header_map_.clear();
    self->recv_body_str_.clear();
    http_parser_url_init(self->parser_url_);
    return 0;
}

int lidig_http_session::on_headers_complete(http_parser* parser) {
    LOGD("***HEADERS COMPLETE***");
    return 0;
}

int lidig_http_session::on_message_complete(http_parser* parser) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    LOGD("***MESSAGE COMPLETE***");
    http_parser_init(self->parser_, HTTP_BOTH);
    //http_parser_url_init(self->parser_url_);
    if (self->http_server_)
        self->http_server_->on_packet(self, self->recv_header_map_,
                self->recv_body_str_.data(),
                self->recv_body_str_.size());

    self->on_packet(self, self->recv_header_map_,
            self->recv_body_str_.data(),
            self->recv_body_str_.size());

    return 0;
}

int lidig_http_session::on_status(http_parser* parser, const char *at, size_t length) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    string str(at, length);
    LogDebug() << parser->status_code << " " << str;
    self->recv_header_map_["Status"] = to_string(parser->status_code);
    self->recv_header_map_["Reason"] = str;

    return 0;
}

std::string lidig_http_session::schema() {
    const auto& field = parser_url_->field_data[UF_SCHEMA];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::host() {
    const auto& field = parser_url_->field_data[UF_HOST];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::port() {
    const auto& field = parser_url_->field_data[UF_PORT];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::path() {
    const auto& field = parser_url_->field_data[UF_PATH];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::query() {
    const auto& field = parser_url_->field_data[UF_QUERY];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::fragment() {
    const auto& field = parser_url_->field_data[UF_FRAGMENT];
    return response_url_.substr(field.off, field.len);
}

std::string lidig_http_session::userInfo() {
    const auto& field = parser_url_->field_data[UF_USERINFO];
    return response_url_.substr(field.off, field.len);
}

int lidig_http_session::on_url(http_parser* parser, const char *at, size_t length) {
    lidig_http_session* self = (lidig_http_session *)(parser->data);
    string str(at, length);
    LogDebug() << http_method_str((enum http_method)parser->method) << " " << str;
    self->recv_header_map_["Method"] = http_method_str((enum http_method)parser->method);
    self->recv_header_map_["Path"] = str;
    self->response_url_ = str;

    return http_parser_parse_url(at, length, 0, self->parser_url_);
}

void lidig_http_session::set_http_server(lidig_http_server* self) {
    http_server_ = self;
}

void lidig_http_session::on_read(lidig_stream* stream, const char* data, ssize_t nread) {
    if (nread <= 0)
        return;

    string str((char*)data, nread);
    LogDebug() << nread << "\n" << str;
    http_parser_execute(parser_, &http_settings_, data, nread);
    if (parser_->http_errno != HPE_OK) {
        LogWarn() << stream << " " << http_errno_name((http_errno)parser_->http_errno);
        http_parser_init(parser_, HTTP_BOTH);
        http_parser_url_init(parser_url_);
        return;
    }
}

void lidig_http_session::add_header(const std::string &key, const std::string &value) {
    send_header_map_[key] = value;
}

void lidig_http_session::add_header(const std::string &key, const ssize_t value) {
    send_header_map_[key] = to_string(value);
}

void lidig_http_session::add_body(const char* data, ssize_t size) {
    send_body_str_.append(data, size);
}

void lidig_http_session::add_body(const std::string &data) {
    send_body_str_.append(data);
}

int lidig_http_session::send_response(const std::string &code, const std::string &status) {
    std::string url = "HTTP/1.1 " + code + " " + status + "\r\n";

    time_t raw_time = time(NULL);
    char date[30] = { 0 };
    tm* time_info = gmtime(&raw_time);
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", time_info);

    add_header("Date", date);
    if (send_header_map_.find("Connection") == send_header_map_.end())
        add_header("Connection", "close");
    add_header("Server", "lidig-httpd");

    return send_data(url);
}

int lidig_http_session::send_data(const std::string &url) {
    if (!send_body_str_.empty())
        add_header("Content-Length", send_body_str_.size());

    string header = url;
    for (auto it = send_header_map_.begin(); it != send_header_map_.end(); it++)
        header += it->first + ": " + it->second + "\r\n";

    header += "\r\n";
    header += send_body_str_;

    send_header_map_.clear();
    send_body_str_.clear();
    method_.clear();
    request_url_.clear();

    LogDebug() << header.size() << "\n" << header;
    return async_write(header.data(), header.size());
}

void lidig_http_session::on_close(lidig_stream* stream) {
    LogDebug() << "Local socket = " << stream << " closed!";
    if (http_server_)
        http_server_->on_close_session(this);
}