#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>
#define __CL_ENABLE_EXCEPTIONS 
#include "CL/cl.hpp"

#include <fstream>
#include <streambuf>

#include <fcntl.h>

#include "../utilities/hpceutils.h"

#if !(defined(_WIN32) || defined(_WIN64))
	#include <unistd.h>
#else
	#include <io.h>
	#include <fcntl.h>

	#define STDIN_FILENO 0
	#define STDOUT_FILENO 1
	#define read _read
	#define write _write
#endif




namespace hpce{
	namespace optimised {

		std::string LoadSource(const char *fileName)
		{
			// Don't forget to change your_login here
		    std::string baseDir="src";
		    if(getenv("HPCE_CL_SRC_DIR")){
		        baseDir=getenv("HPCE_CL_SRC_DIR");
		    }

		    std::string fullName=baseDir+"/"+fileName;

		    // Open a read-only binary stream over the file
		    std::ifstream src(fullName, std::ios::in | std::ios::binary);
		    if(!src.is_open())
		        throw std::runtime_error("LoadSource : Couldn't load cl file from '"+fullName+"'.");

		    // Read all characters of the file into a string
		    return std::string(
		        (std::istreambuf_iterator<char>(src)), // Node the extra brackets.
		        std::istreambuf_iterator<char>()
		    );
		}



		void process(int levels, unsigned w, unsigned h, unsigned /*bits*/, std::vector<uint32_t> &pixels)
		{
			std::vector<uint32_t> buffer(w*h);

			if (levels < 0) {
				for(int i=0;i<std::abs(levels);i++){
					auto in=[&](int x, int y) -> uint32_t { return pixels[y*w+x]; };
					auto out=[&](int x, int y) -> uint32_t & {return buffer[y*w+x]; };
					
					for(unsigned x=0;x<w;x++){
						if(x==0){
							out(0,0)=std::min(in(0,0), std::min(in(0,1), in(1,0)));
							for(unsigned y=1;y<h-1;y++){
								out(0,y)=std::min(in(0,y), std::min(in(0,y-1), std::min(in(1,y), in(0,y+1))));
							}
							out(0,h-1)=std::min(in(0,h-1), std::min(in(0,h-2), in(1,h-1)));
						}else if(x<w-1){
							out(x,0)=std::min(in(x,0), std::min(in(x-1,0), std::min(in(x,1), in(x+1,0))));
							for(unsigned y=1;y<h-1;y++){
								out(x,y)=std::min(in(x,y), std::min(in(x-1,y), std::min(in(x,y-1), std::min(in(x,y+1), in(x+1,y)))));
							}
							out(x,h-1)=std::min(in(x,h-1), std::min(in(x-1,h-1), std::min(in(x,h-2), in(x+1,h-1))));
						}else{
							out(w-1,0)=std::min(in(w-1,0), std::min(in(w-1,1), in(w-2,0)));
							for(unsigned y=1;y<h-1;y++){
								out(w-1,y)=std::min(in(w-1,y), std::min(in(w-1,y-1), std::min(in(w-2,y), in(w-1,y+1))));
							}
							out(w-1,h-1)=std::min(in(w-1,h-1), std::min(in(w-1,h-2), in(w-2,h-1)));
						}
					}
					std::swap(pixels, buffer);
				}
				for(int i=0;i<std::abs(levels);i++){
					auto in=[&](int x, int y) -> uint32_t { return pixels[y*w+x]; };
					auto out=[&](int x, int y) -> uint32_t & {return buffer[y*w+x]; };
					
					for(unsigned x=0;x<w;x++){
						if(x==0){
							out(0,0)=std::max(in(0,0), std::max(in(0,1), in(1,0)));
							for(unsigned y=1;y<h-1;y++){
								out(0,y)=std::max(in(0,y), std::max(in(0,y-1), std::max(in(1,y), in(0,y+1))));
							}
							out(0,h-1)=std::max(in(0,h-1), std::max(in(0,h-2), in(1,h-1)));
						}else if(x<w-1){
							out(x,0)=std::max(in(x,0), std::max(in(x-1,0), std::max(in(x,1), in(x+1,0))));
							for(unsigned y=1;y<h-1;y++){
								out(x,y)=std::max(in(x,y), std::max(in(x-1,y), std::max(in(x,y-1), std::max(in(x,y+1), in(x+1,y)))));
							}
							out(x,h-1)=std::max(in(x,h-1), std::max(in(x-1,h-1), std::max(in(x,h-2), in(x+1,h-1))));
						}else{
							out(w-1,0)=std::max(in(w-1,0), std::max(in(w-1,1), in(w-2,0)));
							for(unsigned y=1;y<h-1;y++){
								out(w-1,y)=std::max(in(w-1,y), std::max(in(w-1,y-1), std::max(in(w-2,y), in(w-1,y+1))));
							}
							out(w-1,h-1)=std::max(in(w-1,h-1), std::max(in(w-1,h-2), in(w-2,h-1)));
						}
					}
					std::swap(pixels, buffer);
				}
			} else {
				for(int i=0;i<std::abs(levels);i++){
					auto in=[&](int x, int y) -> uint32_t { return pixels[y*w+x]; };
					auto out=[&](int x, int y) -> uint32_t & {return buffer[y*w+x]; };
					
					for(unsigned x=0;x<w;x++){
						if(x==0){
							out(0,0)=std::max(in(0,0), std::max(in(0,1), in(1,0)));
							for(unsigned y=1;y<h-1;y++){
								out(0,y)=std::max(in(0,y), std::max(in(0,y-1), std::max(in(1,y), in(0,y+1))));
							}
							out(0,h-1)=std::max(in(0,h-1), std::max(in(0,h-2), in(1,h-1)));
						}else if(x<w-1){
							out(x,0)=std::max(in(x,0), std::max(in(x-1,0), std::max(in(x,1), in(x+1,0))));
							for(unsigned y=1;y<h-1;y++){
								out(x,y)=std::max(in(x,y), std::max(in(x-1,y), std::max(in(x,y-1), std::max(in(x,y+1), in(x+1,y)))));
							}
							out(x,h-1)=std::max(in(x,h-1), std::max(in(x-1,h-1), std::max(in(x,h-2), in(x+1,h-1))));
						}else{
							out(w-1,0)=std::max(in(w-1,0), std::max(in(w-1,1), in(w-2,0)));
							for(unsigned y=1;y<h-1;y++){
								out(w-1,y)=std::max(in(w-1,y), std::max(in(w-1,y-1), std::max(in(w-2,y), in(w-1,y+1))));
							}
							out(w-1,h-1)=std::max(in(w-1,h-1), std::max(in(w-1,h-2), in(w-2,h-1)));
						}
					}
					std::swap(pixels, buffer);
				}
				for(int i=0;i<std::abs(levels);i++){
					auto in=[&](int x, int y) -> uint32_t { return pixels[y*w+x]; };
					auto out=[&](int x, int y) -> uint32_t & {return buffer[y*w+x]; };
					
					for(unsigned x=0;x<w;x++){
						if(x==0){
							out(0,0)=std::min(in(0,0), std::min(in(0,1), in(1,0)));
							for(unsigned y=1;y<h-1;y++){
								out(0,y)=std::min(in(0,y), std::min(in(0,y-1), std::min(in(1,y), in(0,y+1))));
							}
							out(0,h-1)=std::min(in(0,h-1), std::min(in(0,h-2), in(1,h-1)));
						}else if(x<w-1){
							out(x,0)=std::min(in(x,0), std::min(in(x-1,0), std::min(in(x,1), in(x+1,0))));
							for(unsigned y=1;y<h-1;y++){
								out(x,y)=std::min(in(x,y), std::min(in(x-1,y), std::min(in(x,y-1), std::min(in(x,y+1), in(x+1,y)))));
							}
							out(x,h-1)=std::min(in(x,h-1), std::min(in(x-1,h-1), std::min(in(x,h-2), in(x+1,h-1))));
						}else{
							out(w-1,0)=std::min(in(w-1,0), std::min(in(w-1,1), in(w-2,0)));
							for(unsigned y=1;y<h-1;y++){
								out(w-1,y)=std::min(in(w-1,y), std::min(in(w-1,y-1), std::min(in(w-2,y), in(w-1,y+1))));
							}
							out(w-1,h-1)=std::min(in(w-1,h-1), std::min(in(w-1,h-2), in(w-2,h-1)));
						}
					}
					std::swap(pixels, buffer);
				}
			}
		}


		int go_no_opencl(unsigned w, unsigned h, unsigned bits, int levels)
		{
			int heightForBuffer = std::abs(levels)*2;
		
			uint64_t readBufferSize  = uint64_t(w)*bits/8;
			std::vector<uint64_t> rawInputBuffer(readBufferSize);
			std::vector<uint64_t> rawOutputBuffer(readBufferSize);

			uint32_t maxLoc   = (uint64_t(w)*(heightForBuffer*2+1));
			std::vector<uint32_t> pixInputBuffer(maxLoc);
			std::vector<uint32_t> pixIntermediateBuffer(maxLoc);
			std::vector<uint32_t> pixOutputBuffer(maxLoc);
			uint32_t inputLoc = 0;
			uint32_t interLoc = 0;
			uint32_t outptLoc = 0;

			int outHeight = 0;

			for (int i = 0; i < heightForBuffer; i++) {

				if(!utils::read_blob(STDIN_FILENO, readBufferSize, &rawInputBuffer[0]))
					break;	// No more images
				utils::unpack_blob(w, 1, bits, &rawInputBuffer[0], &pixInputBuffer[inputLoc]);	
				inputLoc += w;
			}
			
			while(1){
				if(!utils::read_blob(STDIN_FILENO, readBufferSize, &rawInputBuffer[0]))
					break;	// No more images
				utils::unpack_blob(w, 1, bits, &rawInputBuffer[0], &pixInputBuffer[inputLoc]);
				
				inputLoc += w;
				if (inputLoc >= maxLoc){
					inputLoc = 0;
				}

				interLoc = inputLoc;
				for (int i = 0; i < w*(heightForBuffer*2+1) ; i++) {
					pixIntermediateBuffer[i] = pixInputBuffer[interLoc];
					interLoc++;
					if (interLoc >= maxLoc) {
						interLoc = 0;
					}
				}

				if (outHeight < heightForBuffer) {
					int diff = heightForBuffer - outHeight;
					for (int i = 0; i < heightForBuffer+1+outHeight; i++) {
						for (int j = i*w; j < (i+1)*w; j++) {
							pixIntermediateBuffer[j] = pixIntermediateBuffer[j + diff*w];
						}
					}
					process(levels, w, 2*heightForBuffer+1, bits, pixIntermediateBuffer);

					utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*(heightForBuffer-diff)], &rawOutputBuffer[0]);
					utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
				} else {
					process(levels, w, 2*heightForBuffer+1, bits, pixIntermediateBuffer);

					utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*heightForBuffer], &rawOutputBuffer[0]);
					utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
				}

				outHeight++;
			}

			// write the last few lines
			for (int i = 0; i < heightForBuffer; i++) {
				utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*(heightForBuffer+i+1)], &rawOutputBuffer[0]);
				utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
			}
			
			return 0;
		}





		int go_opencl(unsigned w, unsigned h, unsigned bits, int levels)
		{
			
			std::vector<cl::Platform> platforms;

			cl::Platform::get(&platforms);
			if(platforms.size()==0)
				throw std::runtime_error("No OpenCL platforms found.");

			std::cerr<<"Found "<<platforms.size()<<" platforms\n";
			for(unsigned i=0;i<platforms.size();i++){
			    std::string vendor=platforms[i].getInfo<CL_PLATFORM_VENDOR>();
			    std::cerr<<"  Platform "<<i<<" : "<<vendor<<"\n";
			}
			int selectedPlatform=0;
			if(getenv("HPCE_SELECT_PLATFORM")){
			    selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
			}
			std::cerr<<"Choosing platform "<<selectedPlatform<<"\n";
			cl::Platform platform=platforms.at(selectedPlatform);  


			std::vector<cl::Device> devices;
			platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);  
			if(devices.size()==0){
			    throw std::runtime_error("No opencl devices found.\n");
			}

			std::cerr<<"Found "<<devices.size()<<" devices\n";
			for(unsigned i=0;i<devices.size();i++){
			    std::string name=devices[i].getInfo<CL_DEVICE_NAME>();
			    std::cerr<<"  Device "<<i<<" : "<<name<<"\n";
			}

			int selectedDevice=0;
			if(getenv("HPCE_SELECT_DEVICE")){
			    selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
			}
			std::cerr<<"Choosing device "<<selectedDevice<<"\n";
			cl::Device device=devices.at(selectedDevice);

			cl::Context context(devices);



			std::string kernelSource=LoadSource("process.cl");

			cl::Program::Sources sources;   // A vector of (data,length) pairs
			sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1)); // push on our single string

			cl::Program program(context, sources);
			try{
				program.build(devices);
			}catch(...){
				for(unsigned i=0;i<devices.size();i++){
					std::cerr<<"Log for device "<<devices[i].getInfo<CL_DEVICE_NAME>()<<":\n\n";
					std::cerr<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i])<<"\n\n";
				}
				throw;
			}

			fprintf(stderr, "Kernel Done.\n");
			// use open gl here
			
			
			int heightForBuffer = std::abs(levels)*2;
		
			uint64_t readBufferSize  = uint64_t(w)*bits/8;
			std::vector<uint64_t> rawInputBuffer(readBufferSize);
			std::vector<uint64_t> rawOutputBuffer(readBufferSize);

			uint32_t maxLoc   = (uint64_t(w)*(heightForBuffer*2+1));
			std::vector<uint32_t> pixInputBuffer(maxLoc);
			std::vector<uint32_t> pixIntermediateBuffer(maxLoc);
			std::vector<uint32_t> pixOutputBuffer(maxLoc);
			uint32_t inputLoc = 0;
			uint32_t interLoc = 0;
			uint32_t outptLoc = 0;

			int outHeight = 0;

			for (int i = 0; i < heightForBuffer; i++) {

				if(!utils::read_blob(STDIN_FILENO, readBufferSize, &rawInputBuffer[0]))
					break;	// No more images
				utils::unpack_blob(w, 1, bits, &rawInputBuffer[0], &pixInputBuffer[inputLoc]);	
				inputLoc += w;
			}
			
			while(1){
				if(!utils::read_blob(STDIN_FILENO, readBufferSize, &rawInputBuffer[0]))
					break;	// No more images
				utils::unpack_blob(w, 1, bits, &rawInputBuffer[0], &pixInputBuffer[inputLoc]);
				
				inputLoc += w;
				if (inputLoc >= maxLoc){
					inputLoc = 0;
				}

				interLoc = inputLoc;
				for (int i = 0; i < w*(heightForBuffer*2+1) ; i++) {
					pixIntermediateBuffer[i] = pixInputBuffer[interLoc];
					interLoc++;
					if (interLoc >= maxLoc) {
						interLoc = 0;
					}
				}

				if (outHeight < heightForBuffer) {
					int diff = heightForBuffer - outHeight;
					for (int i = 0; i < heightForBuffer+1+outHeight; i++) {
						for (int j = i*w; j < (i+1)*w; j++) {
							pixIntermediateBuffer[j] = pixIntermediateBuffer[j + diff*w];
						}
					}
					process(levels, w, 2*heightForBuffer+1, bits, pixIntermediateBuffer);

					utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*(heightForBuffer-diff)], &rawOutputBuffer[0]);
					utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
				} else {
					process(levels, w, 2*heightForBuffer+1, bits, pixIntermediateBuffer);

					utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*heightForBuffer], &rawOutputBuffer[0]);
					utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
				}

				outHeight++;
			}

			// write the last few lines
			for (int i = 0; i < heightForBuffer; i++) {
				utils::pack_blob(w, 1, bits, &pixIntermediateBuffer[w*(heightForBuffer+i+1)], &rawOutputBuffer[0]);
				utils::write_blob(STDOUT_FILENO, readBufferSize, &rawOutputBuffer[0]);
			}
			
			return 0;
		}





		int go(unsigned w, unsigned h, unsigned bits, int levels)
		{
			if (w > 1000)  {
				return go_opencl(w, h, bits, levels);
			}
			return go_no_opencl(w, h, bits, levels);

		}




	}
}