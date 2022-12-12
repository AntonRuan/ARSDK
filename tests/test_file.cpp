#include <map>
#include "util/lidig_crc.h"
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

using namespace std;

class test_file : public lidig_file, public lidig_timer
{
public:
    void on_write(lidig_file* file, const char* data) override {

    }

    void on_close(lidig_file* file) override {

    }

    void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        string str(data, nread-1);
        vector<string> s = lidig_crc::split(str, " ");
        if (s.size() == 0)
            return;

        switch(flag_) {
        case 0:
            LogInfo() << "temp:" << stoi(s[0])/1000;
            break;
        case 1:
            LogInfo() << "loadavg:" << stof(s[0]);
            break;
        default:
            break;
        }

        flag_++;

        close();
    }

    void on_time(lidig_timer*) override {
        if (get_fd() != 0)
            return;

        switch(flag_) {
        case 0:
            open("/sys/class/thermal/thermal_zone0/temp", 0);
            async_read(30, 0);
            break;
        case 1:
            open("/proc/loadavg", 0);
            async_read(30, 0);
            break;
        default:
            flag_ = 0;
            break;
        }
    }

private:
    int flag_;
};

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();


    lidig_loop loop;

    test_file file;
    //file.open("/sys/class/thermal/thermal_zone0/temp", 0);
    //file.async_read(30, 0);
    //file_->async_write("4123", 4, 0);
    file.timer_start(1000, 5000);

    // test_file file1;
    // file1.open("/proc/loadavg", 0);
    // file1.async_read(30, 0);
    // file1.timer_start(1000, 5000);

    // test_file file1;
    // file1.readfile("/sys/class/thermal/thermal_zone0/temp");


    //lidig_timer timer;
    //timer.timer_start(0, 5000);

    return loop.run();
}