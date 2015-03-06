#include "CL/cl.hpp"

namespace hpce{
	namespace optimised {
		// only go is public; reast is pretty 
		void process(int levels, unsigned w, unsigned h, unsigned /*bits*/, std::vector<uint32_t> &pixels);
		void process_opencl(cl::Device device, cl::Program program, cl::Context context, int levels, unsigned w, unsigned h, unsigned bits, std::vector<uint32_t> &pixels);
		int go_no_opencl(unsigned w, unsigned h, unsigned bits, int levels);
		int go_opencl(unsigned w, unsigned h, unsigned bits, int levels);

		int go(unsigned w, unsigned h, unsigned bits, int levels);
	}
}