#include <unistd.h>
#include <vector>
#include "base/lidig_cmd.h"
#include "util/lidig_json.h"
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_http_server.h"
#include "network/lidig_tcp_client.h"

using namespace std;

#define HOST_WWW_PATH "./static"
Json::Value root_;


class test_file : public lidig_file
{
public:
    void on_write(lidig_file* file, const char* data) override {
        LogInfo();
        //delete file;
    }

    void on_close(lidig_file* file) override {
        delete file;
    }

    void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        LogInfo() << nread;
        if (nread < 0)
            return;

        if (get_filename() == (string(HOST_WWW_PATH) + "/data.json")) {
            string json(data, nread);
            Json::Value root = lidig_json::string_to_value(json);
            if (root.isObject()) {
                if (root.isMember("meat")) {
                    int n = root["meat"].size();
                    for (int i = 0; i < n; i++) {
                        string tmp = root["meat"][i].asString();
                        int m = root_["meat"].size();
                        int j = 0;
                        for (j = 0; j < m; ++j) {
                            if (root["meat"][i] == root_["meat"][j]) {
                                break;
                            }
                        }
                        if (j == m)
                            root_["meat"].append(tmp);
                        LogDebug() << tmp;
                    }
                }
                if (root.isMember("vegetables")) {
                    int n = root["vegetables"].size();
                    for (int i = 0; i < n; i++) {
                        string tmp = root["vegetables"][i].asString();
                        int m = root_["vegetables"].size();
                        int j = 0;
                        for (j = 0; j < m; ++j) {
                            if (root["vegetables"][i] == root_["vegetables"][j]) {
                                break;
                            }
                        }
                        if (j == m)
                            root_["vegetables"].append(tmp);
                        LogDebug() << tmp;
                    }
                }
            }
            LogInfo() << lidig_json::value_to_string(root_);
        }
        lidig_http_session* session = (lidig_http_session*) file->user_data;
        session->add_body(data, nread);
        session->send_response("200", "OK");
    }
};

class navi_http : public lidig_http_server
{
public:
    void response_200(lidig_http_session* self) {
        self->send_response("200", "OK");
    }

    void response_404(lidig_http_session* self) {
        self->send_response("404", "Not Found");
    }

    void on_packet(lidig_http_session* session,
                std::map<std::string,std::string> map, const char* data, ssize_t nread) override {
        LogInfo() << session->get_remote_ip() <<  " " << session->get_remote_port() <<  " " << map["Path"];

        if (map["Path"] == "/dictList") {
            string filename = string(HOST_WWW_PATH) + "/data.json";
            test_file* file = new test_file();
            file->user_data = session;
            int fd = file->readfile(filename);
            if (fd < 0) {
                delete file;
                response_404(session);
                session->close();
            }
        } else if (map["Path"] == "/post") {
            string json(data, nread);
            LogInfo() << json;
            Json::Value root = lidig_json::string_to_value(json);
            if (root.isObject()) {
                if (root.isObject()) {
                    if (root.isMember("meat")) {
                        int n = root["meat"].size();
                        for (int i = 0; i < n; i++) {
                            string tmp = root["meat"][i].asString();
                            int m = root_["meat"].size();
                            int j = 0;
                            for (j = 0; j < m; ++j) {
                                if (root["meat"][i] == root_["meat"][j]) {
                                    break;
                                }
                            }
                            if (j == m)
                                root_["meat"].append(tmp);
                            LogDebug() << tmp;
                        }
                    }
                    if (root.isMember("vegetables")) {
                        int n = root["vegetables"].size();
                        for (int i = 0; i < n; i++) {
                            string tmp = root["vegetables"][i].asString();
                            int m = root_["vegetables"].size();
                            int j = 0;
                            for (j = 0; j < m; ++j) {
                                if (root["vegetables"][i] == root_["vegetables"][j]) {
                                    break;
                                }
                            }
                            if (j == m)
                                root_["vegetables"].append(tmp);
                            LogDebug() << tmp;
                        }
                    }
                }
            }
            response_200(session);

            string filename = string(HOST_WWW_PATH) + "/data.json";
            test_file* file = new test_file();
            int fd = file->open(filename, O_RDWR);
            if (fd > 0) {
                string write_data = lidig_json::value_to_string(root_);
                LogInfo() << write_data;
                file->async_write(write_data.data(), write_data.size(), 0);
            }

        } else if (map["Path"] != "") {
            string filename = string(HOST_WWW_PATH) + "/index.html";
            if (map["Path"] != "/")
                filename = HOST_WWW_PATH + map["Path"];
            test_file* file = new test_file();
            file->user_data = session;
            int fd = file->readfile(filename);
            if (fd < 0) {
                delete file;
                response_404(session);
                session->close();
            }
        } else {
            response_200(session);
            session->close();
        }

    }
};


int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;

    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "navi_http", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    Json::Value array;
    array[0] = "肉";
    root_["meat"] = array;
    array[0] = "蔬菜";
    root_["vegetables"] = array;

    navi_http http_server;
    http_server.listen("0.0.0.0", 10083);

    LogInfo() << "start!";

    return loop.run();
}