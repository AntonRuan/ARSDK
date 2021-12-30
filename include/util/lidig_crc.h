/*
 *  Created on: 2021/1/8
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_CRC_H__
#define __LIDIG_CRC_H__

#include <vector>
#include <iostream>

class lidig_crc
{
public:
    static uint16_t crc16(const uint8_t *data, size_t len, uint16_t multinomial) {
        uint16_t crc_tmp = 0;
        uint16_t j;
        uint32_t i;

        //做CRC校验
        for (i = 0, crc_tmp = 0xFFFF; i < len; i++) {
            crc_tmp ^= data[i];
            for (j = 0; j < 8; j++) {
                if (crc_tmp & 0x01) {
                    crc_tmp = (crc_tmp >> 1) ^ multinomial;
                } else {
                    crc_tmp >>= 1;
                }
            }
        }
        return crc_tmp;
    }

    static std::vector<std::string> split(const std::string& str, const std::string& delim) {
        std::vector<std::string> res;
        if ("" == str) return res;

        std::string strs = str + delim; //*****扩展字符串以方便检索最后一个分隔出的字符串
        size_t pos;
        size_t size = strs.size();

        for (size_t i = 0; i < size; ++i) {
            pos = strs.find(delim, i); //pos为分隔符第一次出现的位置，从i到pos之前的字符串是分隔出来的字符串
            if ( pos < size) { //如果查找到，如果没有查找到分隔符，pos为string::npos
                std::string s = strs.substr(i, pos - i);//*****从i开始长度为pos-i的子字符串
                res.push_back(s);//两个连续空格之间切割出的字符串为空字符串，这里没有判断s是否为空，所以最后的结果中有空字符的输出，
                i = pos + delim.size() - 1;
            }
        }
        return res;
    }
};

#endif