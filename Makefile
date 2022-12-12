#
# auther: Anton Ruan
#
all:

#export LIDIG_ARCH := "openwrt_mips"
#export LIDIG_AMQP := 1
#export LIDIG_UCI := 1
export LIDIG_MQTT := 1
#export LIDIG_MODBUS := 1
#export LIDIG_SQLITE3 := 1


# 架构相关
ifeq ($(strip $(LIDIG_ARCH)), "openwrt_mips")
export SYSROOT := /home/xiaomi/host_uclibc/openwrt-sdk-mt7621/staging_dir/toolchain-mipsel_24kc_gcc-7.4.0_musl
export CROSS_COMPILE := $(SYSROOT)/bin/mipsel-openwrt-linux-
export STAGING_DIR := /home/xiaomi/host_uclibc/openwrt-sdk-mt7621/staging_dir
else ifeq ($(strip $(LIDIG_ARCH)), "macos")
export SYSROOT := /opt/homebrew/Cellar/libuv/1.42.0
else
export SYSROOT :=
endif


# 交叉编译工具链
export AR := $(CROSS_COMPILE)ar
export CPP := $(CROSS_COMPILE)g++
export CC := $(CROSS_COMPILE)gcc


# Flags
PWD := $(shell pwd)
CPPFLAGS += -I. -I$(PWD)/include -I$(SYSROOT)/include
LDFLAGS += -L$(SYSROOT)/lib
LDFLAGS += -luv -lpthread
CPPFLAGS += -O0 -g -Wall -std=c++11


# 源代码
sources := $(wildcard base/*.cpp)
sources += $(wildcard network/*.cpp)
sources += $(wildcard protocol/*.cpp)
sources += $(wildcard db/*.cpp)
sources += $(wildcard util/*.cpp)
sources += $(wildcard util/lib_json/*.cpp)
sources += $(wildcard *.cpp)

ifndef LIDIG_AMQP
lidig_ampq := network/lidig_amqp.cpp
tmp = $(filter-out $(lidig_ampq), $(sources))
sources := $(tmp)
else
CPPFLAGS += -I$(PWD)/3rdpart/AMQP-CPP/include
endif

ifndef LIDIG_MQTT
mqtt := network/lidig_mqtt.cpp
tmp = $(filter-out $(mqtt), $(sources))
sources := $(tmp)
endif

ifndef LIDIG_UCI
uci := util/lidig_uci.cpp
config := base/lidig_config.cpp
tmp = $(filter-out $(uci), $(sources))
tmp1 = $(filter-out $(config), $(tmp))
sources := $(tmp1)
endif

ifndef LIDIG_MODBUS
modbus := network/lidig_modbus.cpp
tmp = $(filter-out $(modbus), $(sources))
sources := $(tmp)
endif

ifndef LIDIG_SQLITE3
sqlite3 := db/lidig_sqlite3.cpp
tmp = $(filter-out $(sqlite3), $(sources))
sources := $(tmp)
endif


# 依赖相关
.PHONY: all clean distclean tests
all: libarsdk.so libarsdk.a

%.d: %.cpp
	@rm -f $@; \
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(sources:.cpp=.d)

%.o: %.cpp
	$(CPP) $< $(CPPFLAGS) -fPIC -c -o $@

libarsdk.so: $(sources:.cpp=.o)
	$(CPP) $^ $(LDFLAGS) -shared -o $@

libarsdk.a: $(sources:.cpp=.o)
	$(AR) crs $@ $^
	@chmod a+x $@

hiredis-vip:
	$(MAKE) -C 3rdpart/hiredis-vip

AMQP-CPP:
	$(MAKE) -C 3rdpart/AMQP-CPP static
	cp 3rdpart/AMQP-CPP/src/libamqpcpp.a.4.3.11 3rdpart/AMQP-CPP/src/libamqpcpp.a

tests:
	$(MAKE) -C tests

clean:
	@-rm -rf $(sources:.cpp=.o) $(sources:.cpp=.d) libarsdk.so libarsdk.a

distclean:
	$(MAKE) -C 3rdpart/hiredis-vip clean
	$(MAKE) -C 3rdpart/AMQP-CPP clean
	$(MAKE) -C tests clean
	@-rm -rf $(sources:.cpp=.o) $(sources:.cpp=.d) libarsdk.so libarsdk.a
