#
# auther: Anton Ruan
#
all:

SYSROOT := /opt/homebrew/Cellar/libuv/1.42.0

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
sources := $(wildcard *.cpp)


# 依赖相关
.PHONY: all clean
all: $(sources:.cpp=)

%.d: %.cpp
	@rm -f $@; \
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(SOURCES:.cpp=.d) $(sources:.cpp=.d)

%.o: %.cpp
	$(CPP) $< $(CPPFLAGS) -c -o $@

$(sources:.cpp=): %:%.o $(SOURCES:.cpp=.o)
	$(CPP) $^ $(LDFLAGS) -o $@.exe

clean:
	@-rm -rf $(SOURCES:.cpp=.o) $(SOURCES:.cpp=.d) $(sources:.cpp=.d) $(sources:.cpp=.o) $(sources:.cpp=.exe)

