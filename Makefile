CXX = g++

AR = ar

CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -Iinclude -std=c++14 -fPIC

SRCS = $(shell find src/ -type f -name *.cc)

OBJS = $(SRCS:.cc=.o)

LIBS =

LDFLAGS =

LDLIBS = -lpthread -lboost_system -lboost_log

TARGET = libys_srv

TARGET_SO = $(TARGET).so

TARGET_A = $(TARGET).a

TEST_FLAGS = -O2 -g -Wall -fmessage-length=0 -Iinclude -std=c++14

TEST_SRCS = $(shell find test/ -type f -name *.cc)

TEST_LIBS =

TESTS = $(TEST_SRCS:.cc=.test)

CHECKS = $(TESTS:.test=.check)

all: $(TARGET_SO) $(TARGET_A)

$(TARGET_SO): $(OBJS)
	$(CXX) -o $(TARGET_SO) -shared $(LDFLAGS) $(LDLIBS) $(OBJS) $(LIBS)

$(TARGET_A): $(OBJS)
	$(AR) rvs $(TARGET_A) $(OBJS)

install: all
	cp -rv ./include/* /usr/local/include
	cp -v $(TARGET_A) /usr/local/lib/
	cp -v $(TARGET_SO) /usr/local/lib/

clean:
	rm -f $(OBJS) $(TARGET_SO) $(TARGET_A)

test: $(TESTS)

%.test: %.cc $(TARGET_A)
	    $(CXX) -o $@ $(TEST_FLAGS) $(LDFLAGS) $(LDLIBS) $^

%.check: %.test
	    $<

check: $(CHECKS)

cleantest:
	rm -f $(TESTS)

.PHONY: clean all test check

