CPPFLAGS += -I include -W -Wall -std=c++11 -framework OpenCL -O2 -lm

bin/test_opencl : src/test_opencl.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process : src/process.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)




bin/process_ocl_v1_1 : src/process_ocl_v1_1_single_process_function.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_ocl_v1_2 : src/process_ocl_v1_2_single_process_function.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_ocl_v2 : src/process_ocl_v2_open_cl.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)



bin/process_prlrl_v1 : src/process_prlel_v1.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_prlrl_v2 : src/process_prlel_v2.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)




all : bin/process bin/process_ocl_v1_1 bin/process_ocl_v1_2 bin/process_ocl_v2 bin/process_prlrl_v1 bin/process_prlrl_v2

build_test: bin/test_opencl

test: build_test
	./bin/test_opencl
	./test/simple_compare