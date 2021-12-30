#include "base/lidig_logger.h"
#include "protocol/zabbix.h"

using namespace std;

zabbix_protocol::zabbix_protocol() {
    LogTrace();
}

zabbix_protocol::~zabbix_protocol() {
    LogTrace();
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

vector<string> zabbix_protocol::parser_packet(const string& data) {
    vector<string> vec;
    if (data.size() < ZABBIX_HEADER_SIZE) {
        LogWarn() << "Zabbix recv data size to short!";
        return vec;
    }

    zabbix_protocol_head_t head;
    memcpy(&head, data.data(), ZABBIX_HEADER_SIZE);
    string header_data(head.header_data, 4);
    if (header_data != ZABBIX_HEADER) {
        LogWarn() << "Zabbix header data isn't " << ZABBIX_HEADER;
        return vec;
    }

    if (head.send_len + head.reserved_len != data.size() - ZABBIX_HEADER_SIZE) {
        LogWarn() << "Zabbix data size isn't equal";
        return vec;
    }

    string str = data.substr(ZABBIX_HEADER_SIZE, head.send_len);
    LogTrace().write_binary((char*)&head, ZABBIX_HEADER_SIZE) << str;
    string reserved = data.substr(ZABBIX_HEADER_SIZE+head.send_len);
    vec.push_back(reserved);
    vec.push_back(str);

    return vec;
}