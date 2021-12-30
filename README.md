# ld_sdk
一个轻量级嵌入式网关服务器编程框架

##  1. 目录结构
```
├── include      头文件夹
│   ├── base     基础库头文件
│   ├── db       数据库头文件
│   ├── json     json头文件
│   ├── network  通讯库头文件
│   ├── protocol 协议库头文件
│   └── util     工具库头文件
├── base         基础库
├── db           数据库
├── network      通讯库
├── protocol     协议库
├── util         工具库
├── 3rdpart      第三方库
└── tests        测试用例
```


## 2. 项目功能
### 基础库
* 日志读写模块
* 文件配置模块
* 健康检测模块
* 线程模块
* 定时器模块
* 文件模块
* 命令行参数模块

### 数据库
* sqlite3数据库

### 通讯库
* AMQP客户端
* HTTP客户端和服务端
* TCP客户端和服务端
* MQTT客户端
* Modbus客户端
* 串口客户端
* 本地套接字服务端

### 协议库
* ldbd立地二进制协议
* zabbix二进制协议

### 工具库
* base64
* json
* uci

## 3. 编译
* 编译环境
Ubuntu 18.04

* 依赖第三方库
sudo apt install libuv1-dev libmosquittopp-dev libmodbus-dev libsqlite3-dev

* 安装uci
git clone http://git.nbd.name/luci2/libubox.git
git clone https://git.openwrt.org/project/uci.git
cmake -DBUILD_LUA=off
make install
新增依赖库路径 编辑/etc/ld.so.conf档案，在最后一行新增上“/usr/local/lib”
ldconfig 生效

* 获取3rdpart代码及编译
git submodule update --init --recursive
make AMQP-CPP -j8

* 编译
make -j8

* 编译测试用例
make tests -j8

* 生成静态库文件和动态库文件
libldsdk.a
libld_sdk.so

## 4. 用法
参考测试用例