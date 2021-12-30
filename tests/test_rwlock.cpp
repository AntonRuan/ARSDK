#include "base/lidig_loop.h"
#include "base/lidig_thread.h"
#include "base/lidig_logger.h"

uv_rwlock_t lock;
int num = 0;
void writer(void *arg) {
    uv_rwlock_wrlock(&lock);
    sleep(2);
    num++;
    LOGFMTI("num is %d", num);
    uv_rwlock_wrunlock(&lock);
}

void reader(void *arg) {
    uv_rwlock_rdlock(&lock);
    sleep(2);
    LOGFMTI("num is %d", num);
    uv_rwlock_rdunlock(&lock);
}

int main(int argc, char **argv) {
    lidig_logger::get_instance().logger_init();
    lidig_logger::get_instance().add_logger_to_screen(FNLog::PRIORITY_TRACE);
    lidig_logger::get_instance().set_logger_sync();
    lidig_logger::get_instance().start();

    uv_rwlock_init(&lock);
    uv_thread_t nthread1,nthread2,nthread3,nthread4;
    uv_thread_create(&nthread1, writer, NULL);
    uv_thread_create(&nthread2, writer, NULL);
    uv_thread_create(&nthread3, reader, NULL);
    uv_thread_create(&nthread4, reader, NULL);
    uv_thread_join(&nthread1);
    uv_thread_join(&nthread2);
    uv_thread_join(&nthread3);
    uv_thread_join(&nthread4);
}