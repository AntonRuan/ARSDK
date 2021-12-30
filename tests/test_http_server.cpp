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

lidig_loop* loop_;

#define HOST_WWW_PATH "/home/ruan/temp/"
map<lidig_http_session*, int> sessions_map_;
map<string, string> pictures_map_;

void response_401(lidig_http_session* self) {
    string html = "<html><body><h1>Access Error: 401 -- Unauthorized</h1></body></html>\n";
    self->add_header("WWW-Authenticate", "Digest qop=\"auth\", realm=\"IP Camera(D8382)\",\
nonce=\"4e4467794d6a4135593251364d5749344e7a4e6b5a6d4d3d\", stale=\"FALSE\"");
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("401", "Unauthorized");
}

void response_200(lidig_http_session* self) {
    string html = "<html><body><h1>Hello, World!</h1></body></html>\n";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");
}

void response_status(lidig_http_session* self) {
    Json::Value root;
    root["CPU"] = 1;
    root["MEM"] = 0;
    root["uptime"] = 0;

    self->add_header("Content-Type", "application/json");
    self->add_body(lidig_json::value_to_string(root, 0));
    self->send_response("200", "OK");
}

void response_event_capabilities(lidig_http_session* self) {
    Json::Value root;
    root["isSupportFaceCollection"] = 1;
    root["isSupportAccssControl"] = 0;
    root["isSupportVehicleCollection"] = 0;
    root["isSupportIdCardCollection"] = 0;

    self->add_header("Content-Type", "application/json");
    self->add_body(lidig_json::value_to_string(root, 0));
    self->send_response("200", "OK");
}

void response_system_capabilities(lidig_http_session* self) {
    Json::Value root;
    root["isSupportPreview"] = 1;
    root["isSupportSystemMaintain"] = 1;
    root["isSupportReboot"] = 1;
    root["isSupportFactoryReset"] = 1;
    root["isSupport28181"] = 1;
    root["isSupportEhome"] = 1;

    self->add_header("Content-Type", "application/json");
    self->add_body(lidig_json::value_to_string(root, 0));
    self->send_response("200", "OK");
}

void response_interfaces(lidig_http_session* self) {
    Json::Value root;
    root["ipAddress"] = "192.168.90.114";
    root["subnetMask"] = "255.255.255.0";
    root["gateway"] = "192.168.90.1";
    root["MACAddress"] = "54:c4:15:f9:78:cf";

    self->add_header("Content-Type", "application/json");
    self->add_body(lidig_json::value_to_string(root, 0));
    self->send_response("200", "OK");
}

void facecapture(lidig_http_session* session) {
    Json::Value root;
    root["ipAddress"] = session->get_local_ip();
    root["portNo"] = session->get_local_port();
    root["protocol"] = "HTTP";
    root["macAddress"] = "84:9a:40:f3:cc:74";
    root["channelID"] = 1;
    time_t raw_time = time(NULL);
    root["dateTime"] = (int)raw_time;
    root["eventState"] = "active";
    root["eventType"] = "faceCapture";
    session->add_body(lidig_json::value_to_string(root, 0));
    session->add_header("Content-Disposition", "form-data; name=\"faceCapture\"");
    session->add_header("Content-Type", "application/json");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["faceImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"faceImage.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "faceImage");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["backgroundImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"backgroundImage.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "backgroundImage");
    session->send_data("\r\n--boundary\r\n");
}

void accesscontrol(lidig_http_session* session) {
    Json::Value root;
    root["ipAddress"] = session->get_local_ip();
    root["portNo"] = session->get_local_port();
    time_t raw_time = time(NULL);
    root["dateTime"] = (int)raw_time;
    root["eventType"] = "AccessControl";
    root["cardNum"] = "123456789";
    root["picNum"] = 1;

    session->add_body(lidig_json::value_to_string(root, 0));
    session->add_header("Content-Disposition", "form-data; name=\"AccessControl\"");
    session->add_header("Content-Type", "application/json");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["faceImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"AccessControl.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "AccessControl");
    session->send_data("\r\n--boundary\r\n");
}

void barrier(lidig_http_session* session) {
    Json::Value root;
    root["ipAddress"] = session->get_local_ip();
    root["portNo"] = session->get_local_port();
    time_t raw_time = time(NULL);
    root["dateTime"] = (int)raw_time;
    root["eventType"] = "Barrier";
    root["license"] = "蓝浙A12345";
    root["picNum"] = 2;

    session->add_body(lidig_json::value_to_string(root, 0));
    session->add_header("Content-Disposition", "form-data; name=\"Barrier\"");
    session->add_header("Content-Type", "application/json");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["faceImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"BarrierCar.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "BarrierCar");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["backgroundImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"BarrierPlate.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "BarrierPlate");
    session->send_data("\r\n--boundary\r\n");
}

void guestmachine(lidig_http_session* session) {
    Json::Value root;
    root["ipAddress"] = session->get_local_ip();
    root["portNo"] = session->get_local_port();
    time_t raw_time = time(NULL);
    root["dateTime"] = (int)raw_time;
    root["eventType"] = "GuestMachine";
    root["name"] = "张三";
    root["birth"] = "20000101";
    root["addr"] = "葫芦山葫芦岛";
    root["idCard"] = "110122200001013333";
    root["issuingAuthority"] = "葫芦岛公安局";
    root["validPeriod"] = "2020.03.05-2030.03.05";
    root["sex"] = 1;
    root["nation"] = 1;
    root["picNum"] = 2;

    session->add_body(lidig_json::value_to_string(root, 0));
    session->add_header("Content-Disposition", "form-data; name=\"GuestMachine\"");
    session->add_header("Content-Type", "application/json");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["faceImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"idCard.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "idCard");
    session->send_data("\r\n--boundary\r\n");

    session->add_body(pictures_map_["backgroundImage.jpg"]);
    session->add_header("Content-Disposition", "form-data;\
name=\"20191219001303387008xcP08n42APt\"; filename=\"idCardFace.jpg\"");
    session->add_header("Content-Type", "image/jpeg");
    session->add_header("Content-ID", "idCardFace");
    session->send_data("\r\n--boundary\r\n");
}

void response_alertstream(lidig_http_session* self) {
    self->add_header("Content-Type", "multipart/mixed; boundary=boundary");
    self->send_response("200", "OK");

    sessions_map_[self] = 1;
}

void on_close(lidig_http_session* self) {
    sessions_map_.erase(self);
}

void heartbeat_cb(lidig_timer* timer) {
    for (auto it = sessions_map_.begin(); it != sessions_map_.end(); ++it) {
        lidig_http_session* session = it->first;
        Json::Value root;
        root["ipAddress"] = session->get_local_ip();
        root["portNo"] = session->get_local_port();
        root["protocol"] = "HTTP";
        root["macAddress"] = "84:9a:40:f3:cc:74";
        root["channelID"] = 1;
        time_t raw_time = time(NULL);
        char date[30] = {0};
        tm* time_info = gmtime(&raw_time);
        strftime(date, sizeof(date), "%Y-%m-%dT%H:%M:%S+8:00", time_info);
        root["dateTime"] = date;
        root["eventState"] = "inactive";
        root["eventType"] = "videoloss";
        session->add_body(lidig_json::value_to_string(root, 0));
        session->add_header("Content-Type", "application/json");
        session->send_data("\r\n--boundary\r\n");
    }
    //timer->stop();
}

void time_cb(lidig_timer* timer) {
    int ac_count = 0;
    int ba_count = 0;
    int gm_count = 0;
    for (auto it = sessions_map_.begin(); it != sessions_map_.end(); ++it) {
        lidig_http_session* session = it->first;
        int port = session->get_local_port();
        LogInfo() << session << " send data from " << port;
        if (port >= 10000 && port < 20000)
            facecapture(session);
        if (port >= 20000 && port < 30000) {
            accesscontrol(session);
            ac_count++;
        }
        if (port >= 30000 && port < 40000) {
            barrier(session);
            ba_count++;
        }
        if (port >= 40000 && port < 50000) {
            guestmachine(session);
            gm_count++;
        }

        //session->close();
    }
    int sum = ac_count + ba_count + gm_count;
    if (sum != 0)
        LogInfo() << sum << " " << ac_count << " " << ba_count << " " << gm_count;
    //timer->stop();
}

void on_packet(lidig_http_session* self, map<string,string> header_map,
        const char* data, ssize_t nread) {
    static int ca_count = 0;
    static int al_count = 0;
    LogInfo() << self <<  " " << ca_count << " " << al_count << " "
            << self->get_remote_ip() <<  " " << self->get_remote_port() <<  " " << header_map["Path"];
    string str(data, nread);
    LogDebug() << nread << "\n" << str;

    if (header_map.find("Authorization") == header_map.end()) {
        response_401(self);
        self->close();
        return;
    }

    if (self->path() == "/ISAPI/Event/capabilities") {
        response_event_capabilities(self);
        ca_count++;
    }
    else if (self->path() == "/ISAPI/System/capabilities")
        response_system_capabilities(self);
    else if (self->path() == "/ISAPI/System/Network/interfaces")
        response_interfaces(self);
    else if (self->path() == "/ISAPI/System/Status")
        response_status(self);
    else if (self->path() == "/ISAPI/Event/notification/alertStream") {
        response_alertstream(self);
        al_count++;
        return;
    } else
        response_200(self);

    self->close();
}

class test_file : public lidig_file
{
public:
    virtual void on_write(lidig_file* file, const char* data) {

    }

    virtual void on_close(lidig_file* file) {

    }

    virtual void on_read(lidig_file* file, char* data, ssize_t nread) {
        if (nread < 0)
            return;

        string path = file->get_filename();
        path = path.substr(path.rfind("/")+1);
        LogDebug() << path;
        pictures_map_[path] = string(data, nread);
    }
};

class test_http_server : public lidig_http_server
{
public:
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {
        on_packet(session, map, data, nread);
    }

    virtual void on_close(lidig_http_session* session) {
        on_close(session);
    }
};

int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;

    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "server", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    loop_ = new lidig_loop();

    for (int i = 2; i < 5; ++i) {
        for (int j = 0; j < 10000; ++j) {
            test_http_server* http_server = new test_http_server();
            http_server->listen("0.0.0.0", (10000*i)+j);
        }
    }

    string path = "faceImage.jpg";
    test_file* file = new test_file();
    file->readfile(HOST_WWW_PATH + path);

    string path1 = "backgroundImage.jpg";
    test_file* file1 = new test_file();
    file1->readfile(HOST_WWW_PATH + path1);

    lidig_timer timer;
    timer.timer_start(1000, 60000, time_cb);

    //lidig_timer timer1(loop_);
    //timer1.timer_start(2000, 4010, heartbeat_cb);

    LogInfo() << "start!";

    return loop_->run();
}
