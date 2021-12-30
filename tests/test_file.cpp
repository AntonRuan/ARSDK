#include <map>
#include "base/lidig_file.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

using namespace std;


class test_file : public lidig_file
{
public:
    virtual void on_write(lidig_file* file, const char* data) override {

    }

    virtual void on_close(lidig_file* file) override {

    }

    virtual void on_read(lidig_file* file, const char* data, ssize_t nread) override {
        if (nread < 0)
            return;

        printf("%.*s\n", (int)nread, data);
    }
};

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();


    lidig_loop loop;

    test_file* file_;
    file_ = new test_file();
    file_->open("test.txt", O_CREAT);
    file_->async_write("4123", 4, 0);


    test_file file1;
    file1.readfile("test_mqtt.cpp");


    lidig_timer timer;
    timer.timer_start(0, 5000,
        [&](lidig_timer*) {
            //file_->async_write("0123", 4, 0);
            timer.stop();
        }
    );

    return loop.run();
}