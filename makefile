CPPFLAGS += -W -Wall -std=c++11

bin/process : src/process.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

all : bin/process
