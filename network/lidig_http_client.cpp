#include "base/lidig_logger.h"
#include "network/lidig_http_client.h"

using namespace std;

lidig_http_client::lidig_http_client() {
    LogTrace();
}

lidig_http_client::~lidig_http_client() {
    LogTrace();
}

int lidig_http_client::send_request(const std::string &method, const std::string &url) {
    method_ = method;

    http_parser_url parser_url;
    http_parser_url_init(&parser_url);
    if (http_parser_parse_url(url.data(), url.size(), 0, &parser_url) != 0) {
        LogWarn() << url << " url parse error!";
        return -1;
    }

    const auto& field2 = parser_url.field_data[UF_PATH];
    request_url_ = url.substr(field2.off);

    const auto& field = parser_url.field_data[UF_HOST];
    string host = url.substr(field.off, field.len);

    add_header("Connection", "keep-alive");
    add_header("Accept", "*/*");
    add_header("Accept-Language", "zh-CN,zh;q=0.8");
    add_header("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) \
AppleWebKit/537.36 (KHTML, like Gecko) Chrome/57.0.2987.133 Safari/537.36");

    if (host == "") {
        std::string url = method_ + " " + request_url_ + " HTTP/1.1\r\n";
        add_header("Host", ip_);
        return send_data(url);
    }
    add_header("Host", host);

    const auto& field1 = parser_url.field_data[UF_PORT];
    string port = url.substr(field1.off, field1.len);

    if (port == "") port = "80";

    return connect(host, stoi(port));
}

void lidig_http_client::on_connection(lidig_tcp_client* client, int status) {
    on_connection((lidig_http_client*)client, status);
    if (status < 0 || method_ == "")
        return;

    std::string url = method_ + " " + request_url_ + " HTTP/1.1\r\n";
    send_data(url);
}