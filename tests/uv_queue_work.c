
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uv.h"

static int luv_cpu_info();

/*
libuv工作队列：
1.复杂耗时的工作内容放在工作队列，让线程池来处理;
2.线程池处理完后会调传入的回调函数，回到主线程继续处理;
*/

static void uv_work_callback(uv_work_t* req)
{
	//另外一个线程中执行
	printf("work id: %d\n",(int)uv_thread_self());
	sleep(1);
	printf("work data: %d\n", (int)req->data);
	req->data += 2;
}

static void uv_after_work_callback(uv_work_t* req, int status)
{
	//主线程中执行
	printf("after work id: %d\n", (int)uv_thread_self());
	printf("after work data: %d\n", (int)req->data);
}

//每个函数中打印一下线程id
//mian: id = 0;
//uv_work_callback: id = 156;
//uv_after_work_callback: id = 0;
//说明uv_work_callback在另一个线程中执行;
int main(int argc, char** argv)
{
	printf("main id: %d\n", (int)uv_thread_self());

	uv_loop_t *uv_loop = uv_default_loop();
	luv_cpu_info();

	//创建工作数据结构,自定义数据结构添加在data中
	uv_work_t uv_work;
	uv_work.data = (void*)2018;

	//添加工作任务到libuv的队列中，libuv的线程池的线程获得锁后即可处理调用uv_work_callback函数，处理完毕后调用uv_after_work_callback函数
	uv_queue_work(uv_loop, &uv_work, uv_work_callback, uv_after_work_callback);

	uv_run(uv_loop, UV_RUN_DEFAULT);

	//system("pause");
	return 0;
}

static int luv_cpu_info() {
	uv_cpu_info_t* cpu_infos;
	int count, i;
	int ret = uv_cpu_info(&cpu_infos, &count);
	if (ret < 0) return ret;

	printf("uv_version == %s\n", uv_version_string());

	for (i = 0; i < count; i++) {
		printf("%s | ", cpu_infos[i].model);
		printf("%d | ", cpu_infos[i].speed);
		printf("%ld | ", cpu_infos[i].cpu_times.user);
		printf("%ld | ", cpu_infos[i].cpu_times.nice);
		printf("%ld | ", cpu_infos[i].cpu_times.sys);
		printf("%ld | ", cpu_infos[i].cpu_times.idle);
		printf("%ld \n", cpu_infos[i].cpu_times.irq);
	}

	uv_free_cpu_info(cpu_infos, count);
	return 1;
}