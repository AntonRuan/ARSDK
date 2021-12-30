#include <chrono>
#include <sstream>
#include "base/lidig_logger.h"
#include "network/lidig_amqp.h"

lidig_amqp::lidig_amqp() :
        AMQP::LibUvHandler(lidig_loop::get_loop()), lidig_health("AMQP") {
    LogTrace();
    timer_ = new lidig_timer();
    timer_->timer_start(10000, 10000, std::bind(&lidig_amqp::heartbeat_cb, this));
}

lidig_amqp::~lidig_amqp() {
    LogTrace();
    if (channel_)
        delete channel_;
    if (connection_)
        delete connection_;
    if (timer_)
        delete timer_;
}

void lidig_amqp::onError(AMQP::TcpConnection *conn, const char *message) {
    LogError() << address_ + ": " + message;
    disconnect();
    set_sick(address_ + ": " + message);
}

void lidig_amqp::onConnected(AMQP::TcpConnection *conn) {
    LogInfo();
    on_connection(this, 0);
    set_good();
}

void lidig_amqp::disconnect() {
    LogInfo();
    if (connection_)
        connection_->close();

    if (connection_) {
        delete connection_;
        connection_ = nullptr;
    }
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    set_check();
}

void lidig_amqp::heartbeat_cb() {
    if (connection_)
        connection_->heartbeat();
}

void lidig_amqp::bind_queue(std::vector<std::string> v) {
    for (auto it = v.begin(); it != v.end(); ++it)
        bind_queue(*it);
}

void lidig_amqp::bind_queue(const std::string& key) {
    key_vec_.push_back(key);
    if (channel_ == nullptr)
        return;

    channel_->bindQueue("lidig-exchange", queue_name_, key).onSuccess(
        [key](){
            LogInfo() << "Bind " << key << " to queue ";
        }
    );
}

int lidig_amqp::connect(const std::string& address, const std::string& queue) {
    address_ = address;
    queue_name_ = queue;
    connection_ = new AMQP::TcpConnection(this, AMQP::Address(address));
    channel_ = new AMQP::TcpChannel(connection_);
    LogInfo() << "to " << address;

    // need for publish
    channel_->declareExchange("lidig-exchange", AMQP::topic).onSuccess(
        []() {
            LogInfo() << "declareExchange";
        }
    );

    // need for consume
    channel_->declareQueue(queue, AMQP::autodelete).onSuccess(
        [](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
            LogInfo() << "Created queue: " << name;
        }
    );
    channel_->bindQueue("lidig-exchange", queue, queue + ".#").onSuccess(
        [queue](){
            LogInfo() << "Bind " << queue << ".# to queue " << queue;
        }
    );
    channel_->consume(queue).onReceived(
        [&](const AMQP::Message& m, uint64_t deliveryTag, bool) {
            std::string ret;

            ret = on_packet(m.routingkey(), m.body(), m.bodySize(),
                    m.correlationID(), m.replyTo());

            if (!ret.empty() && !m.replyTo().empty()) {
                AMQP::Envelope env(ret.c_str(), ret.size());
                env.setCorrelationID(m.correlationID());
                channel_->publish("", m.replyTo(), env);
            }
            channel_->ack(deliveryTag);
        }
    );

    return 0;
}

void lidig_amqp::publish(const std::string& exchange,
        const std::string& key, const std::string& message, const std::string& id) {
    if (channel_ == nullptr)
        return;

    AMQP::Envelope env(message.c_str(), message.size());
    env.setCorrelationID(id);
    channel_->publish(exchange, key, env);
}

void lidig_amqp::publish(const std::string& key, const char* data, size_t size) {
    std::string message(data, size);
    publish(key, message);
}

void lidig_amqp::publish(const std::string& key, const std::string& message) {
    if (channel_ == nullptr)
        return;

    if (0) {
        std::stringstream ss;
        ss << std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        std::string correlation = ss.str();
        AMQP::Envelope env(message.c_str(), message.size());
        env.setCorrelationID(correlation);
        env.setReplyTo(queue_name_);
        channel_->publish("lidig-exchange", key, env);
    } else
        channel_->publish("lidig-exchange", key, message);
}

int lidig_amqp::health_cure() {
    if (connection_ != nullptr || address_ == "")
        return -1;

    connect(address_, queue_name_);
    for (auto it = key_vec_.begin(); it != key_vec_.end(); ++it) {
        std::string key = *it;
        channel_->bindQueue("lidig-exchange", queue_name_, key).onSuccess(
            [key](){
                LogInfo() << "Bind " << key << " to queue ";
            });
    }
    return 0;
}