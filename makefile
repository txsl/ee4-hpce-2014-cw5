CPPFLAGS += -I include -W -Wall -std=c++11 -framework OpenCL -O2 -lm

bin/test_opencl : src/test_opencl.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process : src/process.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v1_1 : src/process_v1_1_single_process_function.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v1_2 : src/process_v1_2_single_process_function.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v2 : src/process_v2_open_cl.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v3 : src/process_v3.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_v4 : src/process_v4.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

all : bin/process bin/process_v1_1 bin/process_v1_2 bin/process_v2 bin/process_v3 bin/process_v4
