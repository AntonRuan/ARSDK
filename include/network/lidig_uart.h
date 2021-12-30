/*
 *  Created on: 2020/8/26
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_UART_H__
#define __LIDIG_UART_H__

#include <iostream>
#include <functional>
#include "base/lidig_pipe.h"

class lidig_uart : public lidig_pipe
{
public:
    lidig_uart();
    virtual ~lidig_uart();

    int open(const std::string& dev_name, uint32_t baud_rate);

private:
    bool set_speed(uint32_t speed);
    bool set_parity(uint32_t databits, uint8_t stopbits, uint8_t parity);
};

#endif