#include "base/lidig_logger.h"
#include "protocol/zabbix.h"

using namespace std;

zabbix_protocol::zabbix_protocol() {
    LogTrace();
}

zabbix_protocol::~zabbix_protocol() {
    LogTrace();
}

string zabbix_protocol::encode_packet(const char* data, size_t len, const char* reserved, size_t res_len) {
    string p1(data, len);
    string p2(reserved, res_len);
    return encode_packet(p1, p2);
}

string zabbix_protocol::encode_packet(const string& data, const std::string& reserved) {
    zabbix_protocol_head_t head;
    memcpy(head.header_data, ZABBIX_HEADER, 4);
    head.flags = 1;
    head.send_len = data.size();
    head.reserved_len = reserved.size();

    char ptdata[ZABBIX_HEADER_SIZE];
    memcpy(ptdata, &head, ZABBIX_HEADER_SIZE);

    LogTrace().write_binary((char*)&head, ZABBIX_HEADER_SIZE) << data;
    return string(ptdata, ZABBIX_HEADER_SIZE) + data + reserved;
}

int zabbix_protocol::parser_packet(const char* data, size_t len) {
    string str(data, len);
    return parser_packet(str);
}

int zabbix_protocol::parser_packet(const string& data) {
    recv_body_str_.append(data);
    if (recv_body_str_.size() < ZABBIX_HEADER_SIZE) {
        LogWarn() << "Zabbix recv data size to short!";
        return -1;
    }

    zabbix_protocol_head_t head;
    memcpy(&head, recv_body_str_.data(), ZABBIX_HEADER_SIZE);
    string header_data(head.header_data, 4);
    if (header_data != ZABBIX_HEADER) {
        LogWarn() << "Zabbix header data isn't " << ZABBIX_HEADER;
        recv_body_str_.clear();
        return -2;
    }

    if (head.send_len + head.reserved_len != recv_body_str_.size() - ZABBIX_HEADER_SIZE) {
        LogWarn() << "Zabbix data size isn't equal";
        return 1;
    }

    string str = recv_body_str_.substr(ZABBIX_HEADER_SIZE, head.send_len);
    LogTrace().write_binary((char*)&head, ZABBIX_HEADER_SIZE) << str;
    string reserved = recv_body_str_.substr(ZABBIX_HEADER_SIZE+head.send_len);
    on_packet(str, reserved);
    recv_body_str_.clear();
    return 0;
}