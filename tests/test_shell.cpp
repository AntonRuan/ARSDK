#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "base/lidig_config.h"
#include "network/lidig_shell.h"

using namespace std;

int main(int argc, char const *argv[]) {
    map<char, string> cmd = lidig_cmd(argc, argv);
    FNLog::LogPriority level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    lidig_shell shell("0.0.0.0", 9000);
    shell.start();

    cmd::get_instance().add_option("list",
        [&](lidig_shell_client* c, const std::string& str)->int {
            c->async_write("a b c d\n");
            return 0;
        }
    );

    return loop.run();
}