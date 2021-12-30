#include <string.h>
#include "base/lidig_timer.h"
#include "network/lidig_usock_server.h"
#include "network/lidig_tcp_client.h"

using namespace std;

lidig_usock_server* tcp_server_;
lidig_tcp_client* tcp_client_;
lidig_timer* lidig_timer_;
lidig_loop* loop_;

void on_read(lidig_stream*, const char* data, ssize_t nread)
{
    if (nread < 0)
    {
        tcp_client_->close();
        delete(tcp_client_);
        tcp_client_ = nullptr;
        return;
    }
    tcp_client_->async_write(data, nread);
}

int new_connection(lidig_usock_server*, int status)
{
    printf("new_connection %d\n", status);
    tcp_client_ = new lidig_tcp_client();
    tcp_server_->accept(tcp_client_);
    tcp_client_->async_read_start();
    return 0;
}

void time_cb(lidig_timer*)
{
    if (!tcp_client_)
        return;

    string str = "hello world!\n";
    int ret = tcp_client_->async_write(str.c_str(), str.size());
    printf("tick %d\n", ret);
    lidig_timer_->stop();
}

int main(int argc, char const *argv[])
{
    int ret = -1;
    loop_ = new lidig_loop();

    tcp_server_ = new lidig_usock_server(loop_);
    ret = tcp_server_->bind("/tmp/main.sock");
    printf("bind %d\n", ret);
    tcp_server_->listen(new_connection);

    lidig_timer_ = new lidig_timer();
    lidig_timer_->timer_start(0, 1000, time_cb);

    return loop_->run();
}