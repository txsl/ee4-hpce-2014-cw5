#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>


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


///////////////////////////////////////////////////////////////////
// Composite image processing

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

// You may want to play with this to check you understand what is going on
void invert(unsigned w, unsigned h, unsigned bits, std::vector<uint32_t> &pixels)
{
	uint32_t mask=0xFFFFFFFFul>>bits;
	
	for(unsigned i=0;i<w*h;i++){
		pixels[i]=mask-pixels[i];
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

