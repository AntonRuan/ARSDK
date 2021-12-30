#
# auther: Anton Ruan
#
all:

export SYSROOT := /opt/homebrew/Cellar/libuv/1.42.0

# 交叉编译工具链
AR := $(CROSS_COMPILE)ar
CPP := $(CROSS_COMPILE)g++
CC := $(CROSS_COMPILE)gcc

# Flags
CPPFLAGS += -I../include -I$(SYSROOT)/include
LDFLAGS += -L$(SYSROOT)/lib -L..
LDFLAGS += -lldsdk -luv -lpthread
CPPFLAGS += -O0 -g -Wall -std=c++11


# 源代码
sources_tests := $(wildcard *.cpp)


ifdef LIDIG_AMQP
CPPFLAGS += -I../3rdpart/AMQP-CPP/include
LDFLAGS += -L../3rdpart/AMQP-CPP/src -lamqpcpp -ldl
else
ampq := test_amqp.cpp
tmp = $(filter-out $(ampq), $(sources_tests))
sources_tests := $(tmp)
endif

ifdef LIDIG_MQTT
LDFLAGS += -lmosquittopp
endif

ifdef LIDIG_UCI
LDFLAGS += -luci
endif

ifdef LIDIG_MODBUS
LDFLAGS += -lmodbus
endif

ifdef LIDIG_SQLITE3
LDFLAGS += -lsqlite3
else
sqlite3 := test_sqlite3.cpp
tmp = $(filter-out $(sqlite3), $(sources_tests))
sources_tests := $(tmp)
endif


# 依赖相关
.PHONY: all clean
all: $(sources_tests:.cpp=)

%.d: %.cpp
	@rm -f $@; \
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(SOURCES:.cpp=.d) $(sources_tests:.cpp=.d)

%.o: %.cpp
	$(CPP) $< $(CPPFLAGS) -c -o $@

$(sources_tests:.cpp=): %:%.o $(SOURCES:.cpp=.o)
	$(CPP) $^ $(LDFLAGS) -o $@.exe

clean:
	@-rm -rf $(SOURCES:.cpp=.o) $(SOURCES:.cpp=.d) $(sources_tests:.cpp=.d) $(sources_tests:.cpp=.o) $(sources_tests:.cpp=.exe)

