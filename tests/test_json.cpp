#include "util/lidig_json.h"
#include "base/lidig_logger.h"

using namespace std;

string write_Json() {
    std::string jsonStr;
    Json::Value root, array, sub;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;

    root["string"] = "你好";
    //数组
    array[0] = "0";
    array[1] = "1";
    root["array"] = array;
    //子串
    sub["child"] = "childjson";
    root["version"] = sub;

    //1.json单行输出(无换行符), 去掉这句就会格式化输出
    writerBuilder["indentation"] = "";

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root, &os);
    jsonStr = os.str();

    LogInfo() << jsonStr;

    return jsonStr;
}

void read_Json(string sRecv) {
    JSONCPP_STRING errs;
    Json::Value root, array, sub;
    Json::CharReaderBuilder readerBuilder;

    // 这一步可以一步判断json格式是否合法
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    int res = jsonReader->parse(sRecv.c_str(), sRecv.c_str() + sRecv.length(), &root, &errs);
    if (!res || !errs.empty()) {
        std::cout << "parseJson err. " << errs << std::endl;
        return;
    }

    if (root.isArray()) {
        if (root.empty())
            LogDebug() << "[]";
        else {
            int n = root.size();
            for (int i = 0; i < n; i++) {
                string str1 = "command";
                sub = root[i].get(str1.data(), array);
                LogDebug() << sub.asString();
                vector<string> keys = root[i].getMemberNames();
                for (size_t j = 0; j < keys.size(); j++) {
                    const string& key = keys[j];
                    LogDebug() <<  key << " : " << root[i][key].asString();
                }
            }
        }
        return;
    }

    string str = "string";
    //sub = root.find(str.data(), str.data() + str.length());
    sub = root.get(str.data(), array);
    LogDebug() << sub.asString();

    if (root.isMember("string")) {
        string tmp = root["string"].asString();
        LogDebug() << "string : " << tmp;
    }

    if (root.isMember("array")) {
        int n = root["array"].size();
        for (int i = 0; i < n; i++) {
            string tmp = root["array"][i].asString();
            LogDebug() << tmp;
        }
    }

    if (root.isObject()) {
        if (root.empty())
            LogDebug() << "{}";
        else {
            vector<string> keys = root.getMemberNames();
            for (size_t i = 0; i < keys.size(); i++) {
                const string& key = keys[i];
                LogDebug() <<  key;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    string str = write_Json();
    string ss = "[{\"command\":\"restart\",\"value\":\"1\"},{\"command\":\"upload\",\"value\":0}]";
    string sa = "{\"method\":\"/config/set\", \"uuid\":123, \"params\":{\"cardNo\" : \"2254475295\",\"cardValid\" : 1,\"cardType\" : 1,\"userType\" : 0,\"validPeriod\" : \"2020.01.01-2021.01.01\",\"name\" : \"张五\",\"password\" : \"password\",\"facePic\" : \"2380beaee1c39a22455db02651abe7be\"}}";
    read_Json(sa);

    return 0;
}