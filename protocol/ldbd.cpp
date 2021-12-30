#include "base/lidig_logger.h"
#include "protocol/ldbd.h"

using namespace std;

ldbd_protocol::ldbd_protocol() {
    LogTrace();
}

ldbd_protocol::~ldbd_protocol() {
    LogTrace();
}

string ldbd_protocol::encode_head(ldbd_protocol_msg_t* pMsg) {
    pMsg->head.data_len = pMsg->data.size();
    char data[LDBD_HEADER_SIZE];

    ldbd_protocol_head_t head = pMsg->head;
    // 把本地字节序转换为网络字节序
    head.data_format = htons(pMsg->head.data_format);
    head.message_id = htonl(pMsg->head.message_id);
    head.data_len = htonl(pMsg->head.data_len);

    memcpy(data, &head, LDBD_HEADER_SIZE);

    LogTrace().write_binary(data, LDBD_HEADER_SIZE) << pMsg->data;
    return string(data, LDBD_HEADER_SIZE) + pMsg->data;
}

string ldbd_protocol::encode_packet(const string& data, int format, int message_id) {
    ldbd_protocol_msg_t msg_writer;

    msg_writer.head.version = LDBD_VERSION;
    msg_writer.head.data_type = ldbd_type_json;
    msg_writer.head.data_format = format;
    msg_writer.head.message_id = message_id;
    msg_writer.data = data;

    return encode_head(&msg_writer);
}

bool ldbd_protocol::parser_head(ldbd_protocol_msg_t* pMsg, const string& data) {
    if (data.size() < LDBD_HEADER_SIZE)
        return false;

    ldbd_protocol_head_t head;
    memcpy(&head, data.data(), LDBD_HEADER_SIZE);

    pMsg->head = head;
    if (head.version != LDBD_VERSION)
        return false;

    // 把网络字节序转换为本地字节序
    pMsg->head.data_format = htons(head.data_format);
    pMsg->head.message_id = htonl(head.message_id);
    pMsg->head.data_len = htonl(head.data_len);

    if (pMsg->head.data_len != (data.size() - LDBD_HEADER_SIZE))
        return false;

    pMsg->data = data.substr(LDBD_HEADER_SIZE);

    LogTrace().write_binary(data.data(), LDBD_HEADER_SIZE) << pMsg->data;
    return true;
}

string ldbd_protocol::parser_packet(const string& data) {
    ldbd_protocol_msg_t msg_reader;

    if (!parser_head(&msg_reader, data))
        return "";

    return msg_reader.data;
}