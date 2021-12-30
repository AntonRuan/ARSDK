#include <iostream>
#include <uv.h>
#include <thread>
#include <unistd.h>

uv_loop_t *loop;
uv_timer_t gc_req;
uv_timer_t fake_job_req;

using namespace std;

void gc(uv_timer_t *handle) {
    fprintf(stderr, "Freeing unused objects\n");
    std::thread::id tid = std::this_thread::get_id();
    cout << "f1 id=" << tid << endl;
}

void fake_job(uv_timer_t *handle) {
    fprintf(stdout, "Fake job done\n");
    std::thread::id tid = std::this_thread::get_id();
    cout << "f2 id=" << tid << endl;
}

void worker(void *arg) {
    uv_thread_t mthread = *((uv_thread_t *) arg);
    uv_thread_t nthread = uv_thread_self();

    cout << "main id=" << mthread << endl;
    cout << "thread id=" << nthread << endl;
    //cout << uv_thread_equal(&mthread, &nthread) << endl;
    sleep(10);
}

int main(int argc, char** argv)
{
    std::thread::id tid = std::this_thread::get_id();
    cout << "main id=" << tid << endl;

    loop = uv_default_loop();

    uv_timer_init(loop, &gc_req);
    uv_unref((uv_handle_t*) &gc_req);

    uv_timer_start(&gc_req, gc, 0, 1000);

    // could actually be a TCP download or something
    uv_timer_init(loop, &fake_job_req);
    uv_timer_start(&fake_job_req, fake_job, 3000, 0);

    uv_thread_t mthread = uv_thread_self();
    uv_thread_t nthread;
    uv_thread_create(&nthread, worker, &mthread);
    //uv_thread_join(&nthread);

    return uv_run(loop, UV_RUN_DEFAULT);
}