/*
 *  Created on: 2021/1/4
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_MODBUS_H__
#define __LIDIG_MODBUS_H__

#include <iostream>
#include <functional>
#include <modbus/modbus.h>
#include "base/lidig_health.h"
#include "base/lidig_thread.h"


class lidig_modbus : public lidig_health
{
public:
    lidig_modbus();
    ~lidig_modbus();

    int open(const std::string& dev_name, uint32_t baud_rate);
    void close();

    int read_bits(int id, int addr, int nb, uint8_t *dest);
    int read_input_bits(int id, int addr, int nb, uint8_t *dest);
    int read_registers(int id, int addr, int nb, uint16_t *dest);
    int read_input_registers(int id, int addr, int nb, uint16_t *dest);
    int write_bit(int id, int coil_addr, int status);
    int write_register(int id, int reg_addr, int value);
    int write_bits(int id, int addr, int nb, const uint8_t *data);
    int write_registers(int id, int addr, int nb, const uint16_t *data);
    int write_and_read_registers(int id, int write_addr, int write_nb,
                                        const uint16_t *src, int read_addr, int read_nb,
                                        uint16_t *dest);
    int send_raw_request(uint8_t *raw_req, int raw_req_length);
    int receive_from_raw(uint8_t *req);
    modbus_t* get_modbus_ctx() {return ctx_;}

protected:
    int health_cure();
    bool alive(void) {return is_alive_;}

private:
    modbus_t* ctx_;
    std::string dev_name_;
    bool is_alive_;
    lidig_mutex mutex_;
};

#endif