/*
 *  Created on: 2020/11/11
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_JSON_H__
#define __LIDIG_JSON_H__

#include <iostream>
#include "json/json.h"

class lidig_json
{
public:
    static std::string value_to_string(Json::Value& root, bool flags = 1) {
        Json::StreamWriterBuilder writerBuilder;
        std::ostringstream os;

        //1.json单行输出(无换行符), 去掉这句就会格式化输出
        if (flags) writerBuilder["indentation"] = "";

        std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
        jsonWriter->write(root, &os);

        return os.str();
    };

    static Json::Value string_to_value(const std::string& str) {
        JSONCPP_STRING errs;
        Json::Value root;
        Json::CharReaderBuilder readerBuilder;

        // 这一步可以一步判断json格式是否合法
        std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
        int res = jsonReader->parse(str.data(), str.data() + str.length(), &root, &errs);
        if (!res || !errs.empty()) {
            std::cout << "parseJson err.\n" << errs << std::endl;
            return root;
        }

        return root;
    }
};

#endif