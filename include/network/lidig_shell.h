/*
 *  Created on: 2021/1/6
 *      Author: Anton Ruan
 */

#ifndef __CHAN_SHELL_H__
#define __CHAN_SHELL_H__

#include <iostream>
#include <functional>
#include "network/lidig_tcp_client.h"
#include "network/lidig_tcp_server.h"

class lidig_shell;
class lidig_shell_client : public lidig_tcp_client
{
public:
    lidig_shell_client();
    virtual ~lidig_shell_client();

    void set_shell_listener(lidig_shell* self) {shell_listener_ = self;}

protected:
    void on_read(lidig_stream* stream, const char* data, ssize_t nread) override;
    void on_close(lidig_stream* stream) override;

private:
    lidig_shell* shell_listener_;
};


class cmd_option
{
public:
    typedef std::function<int(lidig_shell_client*,const std::string&)> option_callback;
    cmd_option(const char s, const std::string& l, option_callback cb) {
        short_opt = s;
        long_opt = l;
        cb_ = cb;
    }

    ~cmd_option() {}

    int callback(lidig_shell_client* c, const std::string& str) {
        return cb_? cb_(c, str): 0;
    }

private:
    char short_opt;
    std::string long_opt;
    option_callback cb_;
};

class cmd
{
public:
    static cmd &get_instance() {
        static cmd instance;
        return instance;
    }

    void exec(lidig_shell_client* client, const std::string& data) {
        if (!client)
            return;

        //LogInfo().write_binary(data.c_str(), data.size());
        std::string str = data;
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        //LogInfo().write_binary(str.c_str(), str.size());
        if (str == "")
            return;
        auto it = cmds.find(str);
        if (it == cmds.end()) {
            client->async_write("Unsupported command: " + str + "!\n");
            return;
        }

        it->second->callback(client, str);
    }


    typedef std::function<int(lidig_shell_client*, const std::string&)> option_callback;
    void add_option(const std::string& cmd, option_callback cb) {
        cmds[cmd] = new cmd_option('e', cmd, cb);
    }

private:
    cmd() {
        cmd_option* help = new cmd_option('h', "help",
            [&](lidig_shell_client* c, const std::string& str)->int {
                for (auto& it: cmds) {
                    c->async_write(it.first + "\n");
                }
                return 0;
            }
        );
        cmds["help"] = help;
        cmd_option* exit = new cmd_option('e', "exit",
            [](lidig_shell_client* c, const std::string& str)->int {
                c->async_write("Bye bye.\n");
                c->close();
                return 0;
            }
        );
        cmds["exit"] = exit;
    }
    ~cmd() {}
    std::map<std::string, cmd_option*> cmds;
};


class lidig_shell : public lidig_tcp_server
{
public:
    lidig_shell(const std::string& ip, int port);
    virtual ~lidig_shell();

    int start();

    void on_client_close(lidig_shell_client* client);
    void on_client_recv(void* para, const std::string& key, const std::string& data);
    void print_shell_prefix(void* para);

protected:
    void on_connection(lidig_tcp_server* server, int status) override;

private:
    std::string ip_;
    int port_;
};
#endif