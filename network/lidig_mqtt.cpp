#include "network/lidig_mqtt.h"
#include "base/lidig_logger.h"
#include "base/lidig_thread.h"

lidig_mqtt::lidig_mqtt(): mosquittopp(), lidig_health("MQTT"), _mqtt_keepalive(60) {
    LogTrace();
}

lidig_mqtt::lidig_mqtt(const std::string& id, const std::string& host,
        int port, int keepalive) :
                mosquittopp(id.c_str()), lidig_health("MQTT"), _mqtt_host(host),
                _mqtt_id(id), _mqtt_port(port), _mqtt_keepalive(keepalive) {
    LogTrace();
}

lidig_mqtt::~lidig_mqtt() {
    LogTrace();
}

int lidig_mqtt::health_cure() {
    return start();
}

void lidig_mqtt::stop() {
    loop_stop(true);
    disconnect();
    set_check();
}

int lidig_mqtt::connect(const std::string& ip, int port) {
    _mqtt_host = ip;
    _mqtt_port = port;
    return 0;
}

int lidig_mqtt::start() {
    return work_.work(this,
        [](void* arg)->int {
            lidig_mqtt* self = (lidig_mqtt*) arg;
            return self->mqtt_start();
        },
        [](void* arg, int ret) {
            lidig_mqtt* self = (lidig_mqtt*) arg;
            if (ret != 0)
                self->set_sick("start failed!");
        }
    );
}

void lidig_mqtt::async_cb(lidig_async* async, void* arg) {
    async_comm_t* comm = (async_comm_t*) arg;

    if (comm->type == "on_message") {
        comm->self->on_packet(comm->self, comm->topic,
                    (const char*)comm->payload, comm->payloadlen);
        free(comm->payload);
        comm->payload = nullptr;
        comm->payloadlen = 0;
    } else if (comm->type == "on_connect") {
        comm->self->on_connection(comm->self, comm->rc);
        if (comm->rc == 0)
            comm->self->set_good();
    } else if (comm->type == "on_disconnect") {
        LogInfo() << comm->rc;
        comm->self->on_connection(comm->self, comm->rc);
        comm->self->set_sick("disconnect!");
    }

    if (comm)
        delete comm;

    if (async)
        delete async;
}

int lidig_mqtt::mqtt_start() {
    int ret = -1;

    if (_mqtt_host.empty()) {
        LOGW("mqtt host empty");
        return ret;
    }

    if (!_username.empty() && !_password.empty()) {
        ret = username_pw_set(_username.c_str(), _password.c_str());
        if (ret != MOSQ_ERR_SUCCESS) {
            LOGW("username_pw_set fail: " << ret);
            return ret;
        }
    }

    if (!_will_topic.empty())
        will_set(_will_topic.c_str());

    LogInfo() << "connect to " << _mqtt_host << ":" << _mqtt_port;
    ret = mosquittopp::connect(_mqtt_host.c_str(), _mqtt_port, _mqtt_keepalive);
    if (ret != MOSQ_ERR_SUCCESS) {
        LOGW(_mqtt_host + ":" + std::to_string(_mqtt_port)
                + " connect fail: " << ret);
        return ret;
    }

    ret = loop_start();
    if (ret != MOSQ_ERR_SUCCESS) {
        LOGW("loop fail: " << ret);
        return ret;
    }

    return 0;
}

void lidig_mqtt::on_message(const struct mosquitto_message *message) {
    std::string msg = std::string(message->topic) + " : ";
    if (message->payloadlen)
        msg.append((char *)message->payload);
    else
        msg.append("(NULL)");
    LOGD(msg);

    async_comm_t* comm = new async_comm_t();
    comm->self = this;
    comm->type = "on_message";
    comm->topic = message->topic;
    comm->payload = malloc(message->payloadlen);
    memcpy(comm->payload, message->payload, message->payloadlen);
    comm->payloadlen = message->payloadlen;

    lidig_async* async = new lidig_async();
    async->async_init(comm, async_cb);
    async->async_send();
}

void lidig_mqtt::on_connect(int rc) {
    LOGI(_mqtt_host << ":" << _mqtt_port << " connected!");
    std::vector<std::string>::iterator it_sub;
    for (it_sub = topic_sub_.begin(); it_sub != topic_sub_.end(); it_sub++) {
        LOGI("subscribe: " + *it_sub);
        subscribe(NULL, (*it_sub).c_str(), 1);
    }

    async_comm_t* comm = new async_comm_t();
    comm->self = this;
    comm->type = "on_connect";
    comm->rc = rc;

    lidig_async* async = new lidig_async();
    async->async_init(comm, async_cb);
    async->async_send();
}

void lidig_mqtt::on_disconnect(int rc) {
    LOGW(_mqtt_host << ":" << _mqtt_port << " disconnect!");

    async_comm_t* comm = new async_comm_t();
    comm->self = this;
    comm->type = "on_disconnect";
    comm->rc = rc;

    lidig_async* async = new lidig_async();
    async->async_init(comm, async_cb);
    async->async_send();
}