#include <unistd.h>
#include "base/lidig_cmd.h"
#include "base/lidig_timer.h"
#include "base/lidig_logger.h"
#include "db/lidig_sqlite3.h"

using namespace std;

int main(int argc, char const *argv[]) {
    FNLog::LogPriority level = FNLog::PRIORITY_ERROR;
    map<char, string> cmd = lidig_cmd(argc, argv);
    if (cmd.find('i') != cmd.end())
        level = FNLog::PRIORITY_INFO;
    if (cmd.find('d') != cmd.end())
        level = FNLog::PRIORITY_DEBUG;
    if (cmd.find('t') != cmd.end())
        level = FNLog::PRIORITY_TRACE;

    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(level);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    lidig_sqlite3 sql;
    sql.open("test.db");
    if (sql.begin_transaction()) {
        //sql.execute("CREATE TABLE test (\"ID\" INTEGER PRIMARY KEY NOT NULL);");
        //sql.execute("insert into test (ID) values (4);");
        lidig_sqlite3_stmt* st = sql.query("select * from test;");
        while (st->next()) {
            LogInfo() << st->get_column_int(0);
        }
        delete st;
        //sql.commit_transaction();
    }
    sql.close();

    lidig_timer timer;
    timer.timer_start(200, 1000,
        [&](lidig_timer*) {
            timer.stop();
        }
    );

    LogAlarm() << "start!";

    return loop.run();
}