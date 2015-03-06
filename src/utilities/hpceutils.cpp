#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>

#include <fcntl.h>

#if !(defined(_WIN32) || defined(_WIN64))
	#include <unistd.h>
#else
	// http://stackoverflow.com/questions/341817/is-there-a-replacement-for-unistd-h-for-windows-visual-c
	// http://stackoverflow.com/questions/13198627/using-file-descriptors-in-visual-studio-2010-and-windows
	// Note: I could have just included <io.h> and msvc would whinge mightily, but carry on
		
	#include <io.h>
	#include <fcntl.h>

	#define STDIN_FILENO 0
	#define STDOUT_FILENO 1
	#define read _read
	#define write _write
#endif

namespace hpce{
	namespace utils {
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




		void validate(int &argc, char *argv[], unsigned &w, unsigned &h, unsigned &bits, int &levels)
		{
			if(argc<3){
				fprintf(stderr, "Usage: process width height [bits] [levels]\n");
				fprintf(stderr, "   bits=8 by default\n");
				fprintf(stderr, "   levels=1 by default\n");
				exit(1);
			}
			
			w=atoi(argv[1]);
			h=atoi(argv[2]);
			
			bits=8;
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
			
			levels=1;
			if(argc>4){
				levels=atoi(argv[4]);
			}
		}

	}
}