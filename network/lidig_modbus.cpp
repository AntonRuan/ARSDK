#include <string.h>
#include "base/lidig_logger.h"
#include "network/lidig_modbus.h"

lidig_modbus::lidig_modbus(): lidig_health("Modbus") {
    LogTrace();
    is_alive_ = false;
}

lidig_modbus::~lidig_modbus() {
    LogTrace();
}

int lidig_modbus::open(const std::string& dev_name, uint32_t baud_rate) {
    dev_name_ = dev_name;
    ctx_ = modbus_new_rtu(dev_name.data(), baud_rate, 'N', 8, 1);
    if (lidig_logger::get_instance().get_screen_logger_level() == FNLog::PRIORITY_TRACE)
        modbus_set_debug(ctx_, TRUE);

    int ret = modbus_connect(ctx_);
    if (ret == 0) {
        LogInfo() << dev_name << " baud_rate " << baud_rate << " opened!";
        set_good();
        is_alive_ = true;
    } else {
        LogError() << dev_name << " baud_rate " << baud_rate << " failed!";
        is_alive_ = false;
        set_sick(dev_name + " open failed");
    }
    return ret;
}

int lidig_modbus::health_cure() {
    int ret = modbus_connect(ctx_);
    if (ret == 0) {
        set_good();
        is_alive_ = true;
    } else {
        is_alive_ = false;
        set_sick(dev_name_ + " open failed");
    }
    return ret;
}

int lidig_modbus::read_bits(int id, int addr, int nb, uint8_t *dest) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_read_bits(ctx_, addr, nb, dest);
}

int lidig_modbus::read_input_bits(int id, int addr, int nb, uint8_t *dest) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_read_input_bits(ctx_, addr, nb, dest);
}

int lidig_modbus::read_registers(int id, int addr, int nb, uint16_t *dest) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_read_registers(ctx_, addr, nb, dest);
}

int lidig_modbus::read_input_registers(int id, int addr, int nb, uint16_t *dest) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_read_input_registers(ctx_, addr, nb, dest);
}

int lidig_modbus::write_bit(int id, int coil_addr, int status) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_write_bit(ctx_, coil_addr, status);
}

int lidig_modbus::write_register(int id, int reg_addr, int value) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_write_register(ctx_, reg_addr, value);
}

int lidig_modbus::write_bits(int id, int addr, int nb, const uint8_t *data) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_write_bits(ctx_, addr, nb, data);
}

int lidig_modbus::write_registers(int id, int addr, int nb, const uint16_t *data) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_write_registers(ctx_, addr, nb, data);
}

int lidig_modbus::write_and_read_registers(int id, int write_addr, int write_nb,
                                    const uint16_t *src, int read_addr,
                                    int read_nb, uint16_t *dest) {
    lidig_scope_lock lock(&mutex_);
    if (modbus_set_slave(ctx_, id) != 0 || ctx_ == nullptr || !alive())
        return -1;
    return modbus_write_and_read_registers(ctx_, write_addr, write_nb,
                                        src, read_addr, read_nb, dest);
}

int lidig_modbus::send_raw_request(uint8_t *raw_req, int raw_req_length) {
    lidig_scope_lock lock(&mutex_);
    if (ctx_ == nullptr || !alive())
        return -1;
    return modbus_send_raw_request(ctx_, raw_req, raw_req_length);
}

int lidig_modbus::receive_from_raw(uint8_t *req) {
    lidig_scope_lock lock(&mutex_);
    if (ctx_ == nullptr || !alive())
        return -1;
    return modbus_receive_confirmation(ctx_, req);
}

void lidig_modbus::close() {
    modbus_close(ctx_);
    modbus_free(ctx_);
}