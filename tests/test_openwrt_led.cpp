#include "base/lidig_pipe.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"

using namespace std;

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    lidig_pipe power;
    power.open("/sys/class/leds/5g_power/brightness");

    lidig_pipe reset;
    reset.open("/sys/class/leds/5g_reset/brightness");

    lidig_timer timer;
    timer.timer_start(100, 500,
        [&](lidig_timer*) {
            static int flag = 0;
            if (flag == 0) {
                power.async_write("1", 1);
                reset.async_write("1", 1);
                flag = 1;
            } else if (flag == 1) {
                power.async_write("0", 1);
                flag = 2;
            } else if (flag == 2) {
                power.async_write("1", 1);
                flag = 3;
            } else {
                timer.stop();
                power.close();
                reset.close();
            }
        }
    );

    return loop.run();
}