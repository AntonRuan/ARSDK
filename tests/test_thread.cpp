#include <unistd.h>
#include <iostream>
#include "base/lidig_loop.h"
#include "base/lidig_thread.h"
#include "base/lidig_logger.h"

using namespace std;

lidig_mutex mutex_;
int num_ = 0;

void worker(void* arg) {
    lidig_scope_lock lock(&mutex_);
    char* str = (char*) arg;
    cout << str << endl;
    for (int i = 0; i < 10000; ++i)
        num_++;
}

int main(int argc, char const *argv[]) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    lidig_loop loop;

    char* str = (char*)"Hello World";
    lidig_thread nthread;
    nthread.create(str, worker);

    char* str1 = (char*)"nihao shijie";
    lidig_thread nthread1;
    nthread1.create(str1, worker);

    nthread.join();
    nthread1.join();

    cout << num_ << endl;

    return loop.run();
}