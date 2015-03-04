CPPFLAGS += -W -Wall -std=c++11

bin/process : src/process.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v1 : src/process_v1_single_process_function.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v2 : src/process_v2.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v3 : src/process_v3.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v4 : src/process_v4.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

all : bin/process bin/process_v1 bin/process_v2 bin/process_v3 bin/process_v4
