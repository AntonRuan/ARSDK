/*
 *  Created on: 2020/12/23
 *      Author: Anton Ruan
 */

#ifndef __ZABBIX_PROTOCOL__
#define __ZABBIX_PROTOCOL__

#include <string>
#include <arpa/inet.h>
#include <vector>
#include "util/lidig_json.h"

#pragma pack(1)
typedef struct
{
    char header_data[4];
    uint8_t flags;
    uint32_t send_len;
    uint32_t reserved_len;
} zabbix_protocol_head_t;
#pragma pack()

#define ZABBIX_HEADER "ZBXD"
#define ZABBIX_HEADER_SIZE sizeof(zabbix_protocol_head_t)

class zabbix_protocol
{
public:
    zabbix_protocol();
    ~zabbix_protocol();

    int parser_packet(const std::string& data);
    int parser_packet(const char* data, size_t len);
    std::string encode_packet(const std::string& data, const std::string& reserved);
    std::string encode_packet(const char* data, size_t len, const char* reserved, size_t res_len);

protected:
    virtual void on_packet(const std::string& data, const std::string& reserved) {}

private:
    std::string recv_body_str_;
};

#endif