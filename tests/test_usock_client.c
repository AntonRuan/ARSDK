#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;
uv_tty_t tty_stdin,tty_stdout;
uv_pipe_t server;

typedef struct {//该结构存在的意义就是向回调函数传递缓冲区地址，并及时释放
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char*)malloc(suggested_size);
  buf->len = suggested_size;
}

void write_to_stdout_cb(uv_write_t* req, int status){
    if(status){
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
        exit(0);
    }
    free_write_req(req);
}

void read_from_pipe_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf){
    write_req_t *wri = (write_req_t *)malloc(sizeof(write_req_t));
    wri->buf = uv_buf_init(buf->base,nread);
    uv_write((uv_write_t*)wri,(uv_stream_t*)&tty_stdout,&wri->buf,1,write_to_stdout_cb);
}

void write_to_pipe_cb(uv_write_t* req, int status){
    if(status){
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
        exit(0);
    }
    uv_read_start((uv_stream_t*)&server,alloc_buffer,read_from_pipe_cb);//再一次构造缓冲区
    free_write_req(req);//释放动态分配的所有数据
}

void read_from_input_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf){
    write_req_t *wri = (write_req_t *)malloc(sizeof(write_req_t));//实例化新结构，主要2个作用：产生write所需的uv_write_t;临时存储buf（同时提供给回调函数析构方法）便于该数据的及时free
    wri->buf=uv_buf_init(buf->base, nread);//buf复制
    uv_write((uv_write_t*)wri,(uv_stream_t*)&server,&wri->buf,1,write_to_pipe_cb);//需要注意的是write调用的时候&wri->buf必须依然有效！所以这里直接用buf会出现问题！
    //write完成之后当前缓冲区也就失去了意义，于是向回调函数传递buf指针，并由回调函数负责析构该缓冲区
}

int main() {
    loop = uv_default_loop();

    uv_pipe_init(loop, &server, 1);
    uv_tty_init(loop,&tty_stdin,0,1);
    uv_tty_init(loop,&tty_stdout,1,0);

    uv_connect_t conn;
    uv_pipe_connect((uv_connect_t*)&conn,&server,"/tmp/main.sock",NULL);//连接pipe
    uv_read_start((uv_stream_t*)&tty_stdin,alloc_buffer,read_from_input_cb);//从stdin读数据，并由此触发回调,每次alloc_buffer回调产生一个缓冲数据结构uv_buf_t并在堆上分配数据！

    return uv_run(loop, UV_RUN_DEFAULT);
}