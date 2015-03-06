# Makefile adapted from HPCE-CW4, which was in turn adapted from  the Hands On OpenCL course: https://github.com/HandsOnOpenCL

ifndef CPPC
	CPPC=g++
endif

LDLIBS = -lOpenCL

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	CPPC = clang++
	LDLIBS = -framework OpenCL
endif

CPPFLAGS += -I include -W -Wall -std=c++11 -O2 -lm

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



bin/process_buf_v1 : src/process_buf_v1.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

bin/process_buf_v2 : src/process_buf_v2.cpp
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)




all : bin/process bin/process_ocl_v1_1 bin/process_ocl_v1_2 bin/process_ocl_v2 bin/process_buf_v1 bin/process_buf_v2

build_test: bin/test_opencl

test: build_test
	./bin/test_opencl
	./test/simple_compare
