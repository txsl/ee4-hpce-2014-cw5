#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>

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
	namespace standard {
		//////////////////////////////////////////////////////////////////
		// Basic image processing primitives

		uint32_t vmin(uint32_t a, uint32_t b)
		{ return std::min(a,b); }

		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c)
		{ return std::min(a,std::min(b,c)); }

		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
		{ return std::min(std::min(a,d),std::min(b,c)); }

		uint32_t vmin(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
		{ return std::min(e, std::min(std::min(a,d),std::min(b,c))); }


		void erode(unsigned w, unsigned h, const std::vector<uint32_t> &input, std::vector<uint32_t> &output)
		{
			auto in=[&](int x, int y) -> uint32_t { return input[y*w+x]; };
			auto out=[&](int x, int y) -> uint32_t & {return output[y*w+x]; };
			
			for(unsigned x=0;x<w;x++){
				if(x==0){
					out(0,0)=vmin(in(0,0), in(0,1), in(1,0));
					for(unsigned y=1;y<h-1;y++){
						out(0,y)=vmin(in(0,y), in(0,y-1), in(1,y), in(0,y+1));
					}
					out(0,h-1)=vmin(in(0,h-1), in(0,h-2), in(1,h-1));
				}else if(x<w-1){
					out(x,0)=vmin(in(x,0), in(x-1,0), in(x,1), in(x+1,0));
					for(unsigned y=1;y<h-1;y++){
						out(x,y)=vmin(in(x,y), in(x-1,y), in(x,y-1), in(x,y+1), in(x+1,y));
					}
					out(x,h-1)=vmin(in(x,h-1), in(x-1,h-1), in(x,h-2), in(x+1,h-1));
				}else{
					out(w-1,0)=vmin(in(w-1,0), in(w-1,1), in(w-2,0));
					for(unsigned y=1;y<h-1;y++){
						out(w-1,y)=vmin(in(w-1,y), in(w-1,y-1), in(w-2,y), in(w-1,y+1));
					}
					out(w-1,h-1)=vmin(in(w-1,h-1), in(w-1,h-2), in(w-2,h-1));
				}
			}
		}

		uint32_t vmax(uint32_t a, uint32_t b)
		{ return std::max(a,b); }

		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c)
		{ return std::max(a,std::max(b,c)); }

		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
		{ return std::max(std::max(a,d),std::max(b,c)); }

		uint32_t vmax(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
		{ return std::max(e, std::max(std::max(a,d),std::max(b,c))); }

		void dilate(unsigned w, unsigned h, const std::vector<uint32_t> &input, std::vector<uint32_t> &output)
		{
			auto in=[&](int x, int y) -> uint32_t { return input[y*w+x]; };
			auto out=[&](int x, int y) -> uint32_t & {return output[y*w+x]; };
			
			for(unsigned x=0;x<w;x++){
				if(x==0){
					out(0,0)=vmax(in(0,0), in(0,1), in(1,0));
					for(unsigned y=1;y<h-1;y++){
						out(0,y)=vmax(in(0,y), in(0,y-1), in(1,y), in(0,y+1));
					}
					out(0,h-1)=vmax(in(0,h-1), in(0,h-2), in(1,h-1));
				}else if(x<w-1){
					out(x,0)=vmax(in(x,0), in(x-1,0), in(x,1), in(x+1,0));
					for(unsigned y=1;y<h-1;y++){
						out(x,y)=vmax(in(x,y), in(x-1,y), in(x,y-1), in(x,y+1), in(x+1,y));
					}
					out(x,h-1)=vmax(in(x,h-1), in(x-1,h-1), in(x,h-2), in(x+1,h-1));
				}else{
					out(w-1,0)=vmax(in(w-1,0), in(w-1,1), in(w-2,0));
					for(unsigned y=1;y<h-1;y++){
						out(w-1,y)=vmax(in(w-1,y), in(w-1,y-1), in(w-2,y), in(w-1,y+1));
					}
					out(w-1,h-1)=vmax(in(w-1,h-1), in(w-1,h-2), in(w-2,h-1));
				}
			}
		}

		///////////////////////////////////////////////////////////////////
		// Composite image processing

		void process(int levels, unsigned w, unsigned h, unsigned /*bits*/, std::vector<uint32_t> &pixels)
		{
			std::vector<uint32_t> buffer(w*h);
			
			// Depending on whether levels is positive or negative,
			// we flip the order round.
			auto fwd=levels < 0 ? erode : dilate;
			auto rev=levels < 0 ? dilate : erode;
			
			for(int i=0;i<std::abs(levels);i++){
				fwd(w, h, pixels, buffer);
				std::swap(pixels, buffer);
			}
			for(int i=0;i<std::abs(levels);i++){
				rev(w,h,pixels, buffer);
				std::swap(pixels, buffer);
			}
		}










		int go(unsigned w, unsigned h, unsigned bits, int levels)
		{
			uint64_t cbRaw=uint64_t(w)*h*bits/8;
			std::vector<uint64_t> raw(size_t(cbRaw/8));
			
			std::vector<uint32_t> pixels(w*h);
			
			while(1){
				if(!hpce::utils::read_blob(STDIN_FILENO, cbRaw, &raw[0]))
					break;	// No more images
				hpce::utils::unpack_blob(w, h, bits, &raw[0], &pixels[0]);		
				
				process(levels, w, h, bits, pixels);
				//invert(w, h, bits, pixels);
				
				hpce::utils::pack_blob(w, h, bits, &pixels[0], &raw[0]);
				hpce::utils::write_blob(STDOUT_FILENO, cbRaw, &raw[0]);
			}
			
			return 0;
		}
	}
}