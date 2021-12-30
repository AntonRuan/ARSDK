/*
 *  Created on: 2020/8/20
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_LOGGER_H__
#define __LIDIG_LOGGER_H__
#include "util/fn_log.h"

class lidig_logger {
public:
    static lidig_logger &get_instance();

    void set_logger_from_config(const std::string& config_path);

    int logger_init();
    void set_logger_sync();
    void add_logger_to_file(const std::string& fpath, const std::string& fname,
                FNLog::LogPriority level = FNLog::PRIORITY_INFO,
                int limit_size = 1000, int rollback = 4);
    void add_logger_to_screen(FNLog::LogPriority level = FNLog::PRIORITY_INFO);
    void add_logger_to_udp(const std::string& ip, int port,
                FNLog::LogPriority level = FNLog::PRIORITY_INFO);
    FNLog::LogPriority get_screen_logger_level() {return level_;}

    int start(void);

protected:
    lidig_logger();
    ~lidig_logger();

private:
    std::string config_path_;
    FNLog::LogPriority level_;
    int device_id_;
};
#endif
