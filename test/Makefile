ifeq ($(findstring debug,$(MAKECMDGOALS)),debug)
    CXXFLAGS=-g -W -Wall -Wno-unused-parameter  -std=c++0x
else
#    CXXFLAGS=-O2 -W -Wall -Wno-unused-parameter -std=c++0x
	CXXFLAGS=-g -W -Wall -Wno-unused-parameter  -std=c++0x
endif


TARGET_A=../bin/notify_watch
TARGET_B=../bin/tcpserver
TARGET_C=../bin/tcpclient
TARGET_D=../bin/memtest
TARGET_E=../bin/broadcast_s
TARGET_F=../bin/broadcast_c
TARGET_G=../bin/agentcli_t
TARGET_H=../bin/dat2cli_t
TARGET_I=../bin/sendfd


CXX=g++ 

INC= -I../public -DLINUX -DDEF_LINUX

LIB=-pthread -lcurl -lboost_log -lboost_filesystem -lboost_thread -lboost_log_setup -lboost_system -lprotobuf

Example_SRC=$(wildcard ./main.cpp ./Watch.cpp )
Example_OBJS=$(patsubst %.cpp, %.o, $(Example_SRC))

ExampleB_SRC=$(wildcard ./tcpserver.c )
ExampleB_OBJS=$(patsubst %.c, %.o, $(ExampleB_SRC))

ExampleC_SRC=$(wildcard ./tcpclient.c )
ExampleC_OBJS=$(patsubst %.c, %.o, $(ExampleC_SRC))

ExampleD_SRC=$(wildcard ./memtest.c )
ExampleD_OBJS=$(patsubst %.c, %.o, $(ExampleD_SRC))

ExampleE_SRC=$(wildcard ./broadcast_s.cpp )
ExampleE_OBJS=$(patsubst %.cpp, %.o, $(ExampleE_SRC))

ExampleF_SRC=$(wildcard ./broadcast_c.cpp )
ExampleF_OBJS=$(patsubst %.cpp, %.o, $(ExampleF_SRC))

ExampleG_SRC=$(wildcard ./agentcli_t.cpp ./CallBackBase.cpp ../public/wwtiny.cpp ../public/mktdata.pb.cpp ../public/d31data.pb.cpp)
ExampleG_OBJS=$(patsubst %.cpp, %.o, $(ExampleG_SRC))

ExampleH_SRC=$(wildcard ./dat2cli_t.cpp ./dat2cli_supp.cpp ../public/MessageQueue.cpp ../public/CSemaphore.cpp ../public/mktdata.pb.cpp ../public/TcpSocket.cpp)
ExampleH_OBJS=$(patsubst %.cpp, %.o, $(ExampleH_SRC))

ExampleI_SRC=$(wildcard ./sendfd.cpp )
ExampleI_OBJS=$(patsubst %.cpp, %.o, $(ExampleI_SRC))



OBJS= $(Example_OBJS) $(ExampleB_OBJS) $(ExampleC_OBJS) $(ExampleD_OBJS) $(ExampleE_OBJS) $(ExampleF_OBJS) $(ExampleG_OBJS) $(ExampleH_OBJS) $(ExampleI_OBJS)
TARGETS=$(TARGET_A) $(TARGET_B) $(TARGET_C) $(TARGET_D) $(TARGET_E) $(TARGET_F) $(TARGET_G) $(TARGET_H) $(TARGET_I)

.PHONY: debug release clean

release: $(TARGETS)

debug: $(TARGETS)

$(TARGET_A): $(Example_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_B): $(ExampleB_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_C): $(ExampleC_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_D): $(ExampleD_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_E): $(ExampleE_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_F): $(ExampleF_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_G): $(ExampleG_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_H): $(ExampleH_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)
$(TARGET_I): $(ExampleI_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LIB)

%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $(INC) $<

clean:
	rm -fr $(TARGETS)  $(OBJS)


