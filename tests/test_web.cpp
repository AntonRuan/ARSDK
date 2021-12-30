#include <unistd.h>
#include <vector>
#include "util/sha.h"
#include "util/base64.h"
#include "util/lidig_json.h"
#include "base/lidig_cmd.h"
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "network/lidig_http_server.h"
#include "network/lidig_tcp_client.h"

using namespace std;

#define HOST_WWW_PATH "/home/ruan/temp/"

void response_200(lidig_http_session* self) {
    string html = "<html><body><h1>Hello, World!</h1></body></html>\n";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");
}

void response_data(lidig_http_session* self) {
    static int d = 0;
    d++;
    char buffer[200] ={0};
    sprintf(buffer, "<html><body><h1>%d</h1></body></html>\n", d);

    self->add_header("Content-Type", "text/html");
    self->add_body(buffer, strlen(buffer));
    self->send_response("200", "OK");
}

void response_asp(lidig_http_session* self) {
    static const string html =
        R"----(
<!DOCTYPE html>
<html>
<body>

<script type="text/javascript">
    self.setInterval("loadXMLDoc()",1000);
    function loadXMLDoc()
    {
        var xmlhttp;
        xmlhttp = new XMLHttpRequest();

        xmlhttp.onreadystatechange = function()
        {
            if (xmlhttp.readyState==4 && xmlhttp.status==200)
            {
                document.getElementById("myp").innerHTML = xmlhttp.responseText;
            }
        }
        xmlhttp.open("POST", "/index.html", true);
        xmlhttp.send();
    }
</script>

<form action="asp">
    First name:<br>
    <input type="text" name="firstname" value="Mickey">
    <br><br>
    <input type="submit" value="Submit">
    <p id="myp"></p>
</form>

</body>
</html>)----";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");

}

void response_101(lidig_http_session* self, const string& sha) {
    char* out;
    int s = BASE64_ENCODE_OUT_SIZE(sha.size());
    out = (char*)malloc(s);
    int size = base64_encode((const unsigned char*)sha.data(), sha.size(), out);
    LogInfo()<< s << " " << size;
    LogInfo() << out;

    self->add_header("Connection", "Upgrade");
    self->add_header("Upgrade", "websocket");
    self->add_header("Sec-WebSocket-Accept", out);
    self->send_response("101", "Switching Protocols");
}

void response_ws(lidig_http_session* self) {
        static const string html =
        R"----(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
</head>
<body>
    <div>
        <input type="submit" value="Start" onclick="start()" />
    </div>
    <div id="messages"></div>
    <script type="text/javascript">
        //var webSocket =  new WebSocket('ws://localhost:8080/testWebSocketProtocol/websocket');
        var webSocket =  new WebSocket('ws://192.168.31.221:9002');

        webSocket.onerror = function(event) {
            onError(event)
        };

        webSocket.onopen = function(event) {
            onOpen(event)
        };

        webSocket.onmessage = function(event) {
            onMessage(event)
        };

        function onMessage(event) {
            document.getElementById('messages').innerHTML
                += '<br />Get Message From Server: ' + event.data;
        }

        function onOpen(event) {
            document.getElementById('messages').innerHTML
                = 'Connection established';
        }

        function onError(event) {
            alert(event.data);
            alert("error");
        }

        function start() {
            webSocket.send('hello');
            return false;
        }
    </script>
</body>
</html>)----";
    self->add_header("Content-Type", "text/html");
    self->add_body(html);
    self->send_response("200", "OK");
}

void time_cb(lidig_timer* timer) {

}

class test_http_server : public lidig_http_server
{
public:
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {
        string path = session->path();
        LogInfo() << session->get_remote_ip() <<  " " << session->get_remote_port() << " " << path;

        //string str(data, nread);
        //LogDebug() << nread << "\n" << str;

        //LogInfo() << session->query();

        if (path.find("asp") != string::npos) {
            response_200(session);
        } else if (path.find("index") != string::npos) {
            response_data(session);
        } else if (path.find("ws") != string::npos) {
            response_ws(session);
        } else {
            response_200(session);
        }

        //response_asp(session);
        session->close();
    }
};

class test_ws_server : public lidig_http_server
{
public:
    virtual void on_packet(lidig_http_session* session,
            std::map<std::string,std::string> map, const char* data, ssize_t nread) {
        string path = session->path();
        LogInfo() << session->get_remote_ip() <<  " " << session->get_remote_port() << " " << path;

        string ws_key = map["Sec-WebSocket-Key"];
        string sha = SHA1::encode_bin(ws_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

        response_101(session, sha);
    }
};

int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;

    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().add_logger_to_file("/tmp", "server", FNLog::PRIORITY_DEBUG);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    test_http_server* http_server = new test_http_server();
    http_server->listen("0.0.0.0", 10000);

    test_ws_server* ws = new test_ws_server();
    ws->listen("0.0.0.0", 9002);

    lidig_timer timer;
    timer.timer_start(1000, 60000, time_cb);

    LogInfo() << "start!";

    return loop.run();
}
