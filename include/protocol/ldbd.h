/*
 *  Created on: 2020/5/28
 *      Author: Anton Ruan
 */

#ifndef __LDBD_PROTOCOL__
#define __LDBD_PROTOCOL__

#include <string>
#include <arpa/inet.h>
#include "util/lidig_json.h"

enum {
    ldbd_type_json = 1,
    __ldbd_type_max,
};

enum {
    ldbd_format_json_respond,       // 应答
    ldbd_format_json_test_ctl,      // 测试的控制包
    ldbd_format_json_test_htbeat,   // 测试的心跳包
    ldbd_format_json_ldpiod_ctl,    // 重合闸控制包
    ldbd_format_json_ldpiod_htbeat, // 重合闸心跳包
    ldbd_format_json_ldsc04_ctl,    // 网络箱控制包
    ldbd_format_json_ldsc04_htbeat, // 网络箱心跳包
    ldbd_format_json_ldheg01_htbeat = 11,// 边缘网关心跳包
    ldbd_format_json_heartbeat = 20,          // 心跳包
    ldbd_format_json_ctl,                         // 老版控制包
    ldbd_format_json_control,                    // 设备控制包
    ldbd_format_json_register,                    // 设备注册包
    ldbd_format_json_property,                    // 设备属性包
    ldbd_format_json_event,                    // 设备事件包
    ldbd_format_json_data,                     // 数据包
    ldbd_format_json_ota,                     // 固件升级包
    ldbd_format_json_log,                     // 获取日志包
    ldbd_format_json_rpc,                     // 远程函数调用包
    __ldbd_format_max,
};

enum {
    code_success = 200,       //设置成功
    code_fail                 //设置失败
};

enum {
    set_success = 0,          //设置成功
    set_fail = 1,             //设置失败
    set_invalid_item = 8,     //未定义的设置项
    set_invalid_range = 10,   //输入范围无效
    set_lack_args = 11        //缺少参数
};

typedef struct {
    uint8_t version;
    uint8_t data_type;
    uint16_t data_format;
    uint32_t message_id;
    uint32_t data_len;
} ldbd_protocol_head_t;

typedef struct
{
    ldbd_protocol_head_t head;
    std::string data;
} ldbd_protocol_msg_t;

#define LDBD_VERSION    1
#define LDBD_HEADER_SIZE sizeof(ldbd_protocol_head_t)


class ldbd_protocol
{
public:
    ldbd_protocol();
    ~ldbd_protocol();

    static std::string parser_packet(const std::string& data);
    static std::string encode_packet(const std::string& data, int format, int message_id);

protected:

private:
    static std::string encode_head(ldbd_protocol_msg_t* pMsg);
    static bool parser_head(ldbd_protocol_msg_t* pMsg, const std::string& data);
};

#endif