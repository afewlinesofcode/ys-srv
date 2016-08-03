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

TEST_LDFLAGS =

TEST_LDLIBS = -Wl,-Bstatic -lys_util -Wl,-Bdynamic -lpthread -lboost_system -lboost_log

TESTS = $(TEST_SRCS:.cc=.test)

CHECKS = $(TESTS:.test=.check)

all: $(TARGET_SO) $(TARGET_A)

$(TARGET_SO): $(OBJS)
	$(CXX) -o $(TARGET_SO) -shared $(LDFLAGS) $(LDLIBS) $(OBJS) $(LIBS)

$(TARGET_A): $(OBJS)
	$(AR) rvs $(TARGET_A) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET_SO) $(TARGET_A)

install: all
	cd include && \
		find . -type f \( -name *.h -o -name *.tcc \) \
			-exec install -v -DT -m 0644 {} /usr/local/include/{} \;
	install -v -t /usr/local/lib/ $(TARGET_A) $(TARGET_SO)

uninstall:
	cd include && \
		find . -type f \( -name *.h -o -name *.tcc \) \
			-exec rm -fv /usr/local/include/{} \;
	cd /usr/local/lib && \
		rm -fv $(TARGET_A) $(TARGET_SO)

test: $(TESTS)

%.test: %.cc
	    $(CXX) $< -o $@ $(TEST_FLAGS) $(TEST_LDFLAGS) $(TEST_LDLIBS) $(TEST_LIBS) $(TARGET_A)

%.check: %.test
	    $<

check: $(CHECKS)

cleantest:
	rm -f $(TESTS)

.PHONY: clean all test check

