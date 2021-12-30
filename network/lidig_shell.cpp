#include "base/lidig_logger.h"
#include "network/lidig_shell.h"

using namespace std;

lidig_shell_client::lidig_shell_client() {
    LogTrace();
}

lidig_shell_client::~lidig_shell_client() {
    LogTrace();
}

void lidig_shell_client::on_read(lidig_stream* stream, const char* data, ssize_t nread) {
    string ip = stream->get_remote_ip();
    if (shell_listener_)
        shell_listener_->on_client_recv(stream, ip, string(data, nread));
}

void lidig_shell_client::on_close(lidig_stream* stream) {
    if (shell_listener_)
        shell_listener_->on_client_close(this);
}


lidig_shell::lidig_shell(const string& ip, int port):
            ip_(ip), port_(port) {
    LogTrace();
}

lidig_shell::~lidig_shell() {
    LogTrace();
}

void lidig_shell::print_shell_prefix(void* para) {
    lidig_shell_client* client = (lidig_shell_client*) para;
    client->async_write("# ");
}

void lidig_shell::on_connection(lidig_tcp_server* server, int status) {
    lidig_shell_client* client = new lidig_shell_client();
    client->set_shell_listener(this);

    if (accept(client) == 0)
        client->async_read_start();
    else
        delete client;

    print_shell_prefix(client);
}

void lidig_shell::on_client_close(lidig_shell_client* client) {
    if (client) delete client;
}

void lidig_shell::on_client_recv(void* para, const string& key, const string& data) {
    lidig_shell_client* client = (lidig_shell_client*) para;

    cmd::get_instance().exec(client, data);
    print_shell_prefix(client);
}

int lidig_shell::start() {
    return listen(ip_, port_);
}