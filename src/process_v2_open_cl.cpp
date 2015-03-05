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


#if !(defined(_WIN32) || defined(_WIN64))
#include <unistd.h>
void set_binary_io()
{}
#else
// http://stackoverflow.com/questions/341817/is-there-a-replacement-for-unistd-h-for-windows-visual-c
// http://stackoverflow.com/questions/13198627/using-file-descriptors-in-visual-studio-2010-and-windows
// Note: I could have just included <io.h> and msvc would whinge mightily, but carry on
	
#include <io.h>
#include <fcntl.h>

#define read _read
#define write _write
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

void set_binary_io()
{
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
}
#endif


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


////////////////////////////////////////////
// Routines for bringing in binary images

/*! Reverse the orders of bits if necessary
	\note This is laborious and a bit pointless. I'm sure it could be removed, or at least moved...
*/
uint64_t shuffle64(unsigned bits, uint64_t x)
{
	if(bits==1){
		x=((x&0x0101010101010101ull)<<7)
			| ((x&0x0202020202020202ull)<<5)
			| ((x&0x0404040404040404ull)<<3)
			| ((x&0x0808080808080808ull)<<1)
			| ((x&0x1010101010101010ull)>>1)
			| ((x&0x2020202020202020ull)>>3)
			| ((x&0x4040404040404040ull)>>5)
			| ((x&0x8080808080808080ull)>>7);
	}else if(bits==2){
		x=((x&0x0303030303030303ull)<<6)
			| ((x&0x0c0c0c0c0c0c0c0cull)<<2)
			| ((x&0x3030303030303030ull)>>2)
			| ((x&0xc0c0c0c0c0c0c0c0ull)>>6);
	}else if(bits==4){
		x=((x&0x0f0f0f0f0f0f0f0full)<<4)
			| ((x&0xf0f0f0f0f0f0f0f0ull)>>4);
	}
	return x;
}

/*! Take data packed into incoming format, and exand to one integer per pixel */
void unpack_blob(unsigned w, unsigned h, unsigned bits, const uint64_t *pRaw, uint32_t *pUnpacked)
{
	uint64_t buffer=0;
	unsigned bufferedBits=0;
	
	const uint64_t MASK=0xFFFFFFFFFFFFFFFFULL>>(64-bits);
	
	for(unsigned i=0;i<w*h;i++){
		if(bufferedBits==0){
			buffer=shuffle64(bits, *pRaw++);
			bufferedBits=64;
		}
		
		pUnpacked[i]=uint32_t(buffer&MASK);
		buffer=buffer>>bits;
		bufferedBits-=bits;
	}
	
	assert(bufferedBits==0);
}

/*! Go back from one integer per pixel to packed format for output. */
void pack_blob(unsigned w, unsigned h, unsigned bits, const uint32_t *pUnpacked, uint64_t *pRaw)
{
	uint64_t buffer=0;
	unsigned bufferedBits=0;
	
	const uint64_t MASK=0xFFFFFFFFFFFFFFFFULL>>(64-bits);
	
	for(unsigned i=0;i<w*h;i++){
		buffer=buffer | (uint64_t(pUnpacked[i]&MASK)<< bufferedBits);
		bufferedBits+=bits;
		
		if(bufferedBits==64){
			*pRaw++ = shuffle64(bits, buffer);
			buffer=0;
			bufferedBits=0;
		}
	}
	
	assert(bufferedBits==0);
}

bool read_blob(int fd, uint64_t cbBlob, void *pBlob)
{
	uint8_t *pBytes=(uint8_t*)pBlob;
	
	uint64_t done=0;
	while(done<cbBlob){
		int todo=(int)std::min(uint64_t(1)<<30, cbBlob-done);		
		
		int got=read(fd, pBytes+done, todo);
		if(got==0 && done==0)
			return false;	// end of file
		if(got<=0){
			throw std::invalid_argument("Read failure.");
		}
		done+=got;
	}
	
	return true;
}

void write_blob(int fd, uint64_t cbBlob, const void *pBlob)
{
	const uint8_t *pBytes=(const uint8_t*)pBlob;
	
	uint64_t done=0;
	while(done<cbBlob){
		int todo=(int)std::min(uint64_t(1)<<30, cbBlob-done);
		
		int got=write(fd, pBytes+done, todo);
		if(got<=0)
			throw std::invalid_argument("Write failure.");
		done+=got;
	}
}

// You may want to play with this to check you understand what is going on
void invert(unsigned w, unsigned h, unsigned bits, std::vector<uint32_t> &pixels)
{
	uint32_t mask=0xFFFFFFFFul>>bits;
	
	for(unsigned i=0;i<w*h;i++){
		pixels[i]=mask-pixels[i];
	}
}




void erode(unsigned x, 
	unsigned w, 
	unsigned h, 
	unsigned *input, 
	unsigned *output)
{
	if(x==0){
		output[w*(0)+0]=std::min(input[w*(0)+0], std::min(input[w*(1)+0], input[w*(0)+1]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+0]=std::min(input[w*(y)+0], std::min(input[w*(y-1)+0], std::min(input[w*(y)+1], input[w*(y+1)+0])));
		}
		output[w*(h-1)+0]=std::min(input[w*(h-1)+0], std::min(input[w*(h-2)+0], input[w*(h-1)+1]));
	}else if(x<w-1){
		output[w*(0)+x]=std::min(input[w*(0)+x], std::min(input[w*(0)+x-1], std::min(input[w*(1)+x], input[w*(0)+x+1])));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+x]=std::min(input[w*(y)+x], std::min(input[w*(y)+x-1], std::min(input[w*(y-1)+x], std::min(input[w*(y+1)+x], input[w*(y)+x+1]))));
		}
		output[w*(h-1)+x]=std::min(input[w*(h-1)+x], std::min(input[w*(h-1)+x-1], std::min(input[w*(h-2)+x], input[w*(h-1)+x+1])));
	}else{
		output[w*(0)+w-1]=std::min(input[w*(0)+w-1], std::min(input[w*(1)+w-1], input[w*(0)+w-2]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+w-1]=std::min(input[w*(y)+w-1], std::min(input[w*(y-1)+w-1], std::min(input[w*(y)+w-2], input[w*(y+1)+w-1])));
		}
		output[w*(h-1)+w-1]=std::min(input[w*(h-1)+w-1], std::min(input[w*(h-2)+w-1], input[w*(h-1)+w-2]));
	}
}

void dilate(unsigned x, 
	unsigned w, 
	unsigned h, 
	unsigned *input, 
	unsigned *output)
{
	if(x==0){
		output[w*(0)+0]=std::max(input[w*(0)+0], std::max(input[w*(1)+0], input[w*(0)+1]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+0]=std::max(input[w*(y)+0], std::max(input[w*(y-1)+0], std::max(input[w*(y)+1], input[w*(y+1)+0])));
		}
		output[w*(h-1)+0]=std::max(input[w*(h-1)+0], std::max(input[w*(h-2)+0], input[w*(h-1)+1]));
	}else if(x<w-1){
		output[w*(0)+x]=std::max(input[w*(0)+x], std::max(input[w*(0)+x-1], std::max(input[w*(1)+x], input[w*(0)+x+1])));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+x]=std::max(input[w*(y)+x], std::max(input[w*(y)+x-1], std::max(input[w*(y-1)+x], std::max(input[w*(y+1)+x], input[w*(y)+x+1]))));
		}
		output[w*(h-1)+x]=std::max(input[w*(h-1)+x], std::max(input[w*(h-1)+x-1], std::max(input[w*(h-2)+x], input[w*(h-1)+x+1])));
	}else{
		output[w*(0)+w-1]=std::max(input[w*(0)+w-1], std::max(input[w*(1)+w-1], input[w*(0)+w-2]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+w-1]=std::max(input[w*(y)+w-1], std::max(input[w*(y-1)+w-1], std::max(input[w*(y)+w-2], input[w*(y+1)+w-1])));
		}
		output[w*(h-1)+w-1]=std::max(input[w*(h-1)+w-1], std::max(input[w*(h-2)+w-1], input[w*(h-1)+w-2]));
	}
}








void process(int levels, unsigned w, unsigned h, unsigned /*bits*/, std::vector<uint32_t> &pixels)
{
	std::vector<uint32_t> buffer(w*h);

	if (levels < 0) {
		for(int i=0;i<std::abs(levels);i++){
			for(unsigned x=0;x<w;x++){
				erode(x, w, h, &pixels[0], &buffer[0]);
			}
			std::swap(pixels, buffer);
		}
		for(int i=0;i<std::abs(levels);i++){
			for(unsigned x=0;x<w;x++){
				dilate(x, w, h, &pixels[0], &buffer[0]);
			}
			std::swap(pixels, buffer);
		}
	} else {
		for(int i=0;i<std::abs(levels);i++){
			for(unsigned x=0;x<w;x++){
				dilate(x, w, h, &pixels[0], &buffer[0]);
			}
			std::swap(pixels, buffer);
		}
		for(int i=0;i<std::abs(levels);i++){
			for(unsigned x=0;x<w;x++){
				erode(x, w, h, &pixels[0], &buffer[0]);
			}
			std::swap(pixels, buffer);
		}
	}
}


int main(int argc, char *argv[])
{
	try{
		if(argc<3){
			fprintf(stderr, "Usage: process width height [bits] [levels]\n");
			fprintf(stderr, "   bits=8 by default\n");
			fprintf(stderr, "   levels=1 by default\n");
			exit(1);
		}
		
		unsigned w=atoi(argv[1]);
		unsigned h=atoi(argv[2]);
		
		unsigned bits=8;
		if(argc>3){
			bits=atoi(argv[3]);
		}
		
		if(bits>32)
			throw std::invalid_argument("Bits must be <= 32.");
		
		unsigned tmp=bits;
		while(tmp!=1){
			tmp>>=1;
			if(tmp==0)
				throw std::invalid_argument("Bits must be a binary power.");
		}
		
		if( ((w*bits)%64) != 0){
			throw std::invalid_argument(" width*bits must be divisible by 64.");
		}
		
		int levels=1;
		if(argc>4){
			levels=atoi(argv[4]);
		}
		
		fprintf(stderr, "Processing %d x %d image with %d bits per pixel.\n", w, h, bits);
		
		uint64_t cbRaw=uint64_t(w)*h*bits/8;
		std::vector<uint64_t> raw(size_t(cbRaw/8));
		
		std::vector<uint32_t> pixels(w*h);
		
		set_binary_io();



		// SET UP OPEN CL

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



		std::string kernelSource=LoadSource("process_v2.cl");

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


		while(1){
			if(!read_blob(STDIN_FILENO, cbRaw, &raw[0]))
				break;	// No more images
			unpack_blob(w, h, bits, &raw[0], &pixels[0]);		
			
			process(levels, w, h, bits, pixels);
			//invert(w, h, bits, pixels);
			
			pack_blob(w, h, bits, &pixels[0], &raw[0]);
			write_blob(STDOUT_FILENO, cbRaw, &raw[0]);
		}
		
		return 0;
	}catch(std::exception &e){
		std::cerr<<"Caught exception : "<<e.what()<<"\n";
		return 1;
	}
}

