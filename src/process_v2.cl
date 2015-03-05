// __kernel void dilate(unsigned x, 
// 	unsigned w, 
// 	unsigned h, 
// 	__global unsigned *input, 
// 	__global unsigned *output)
// {
// 	auto in=[&](int x, int y) -> uint32_t { return input[y*w+x]; };
// 	auto out=[&](int x, int y) -> uint32_t & {return output[y*w+x]; };

// 	if(x==0){
// 		output[0]=std::min(in(0,0), std::min(in(0,1), in(1,0)));
// 		for(unsigned y=1;y<h-1;y++){
// 			output[]0,y)=std::min(in(0,y), std::min(in(0,y-1), std::min(in(1,y), in(0,y+1))));
// 		}
// 		output[]0,h-1)=std::min(in(0,h-1), std::min(in(0,h-2), in(1,h-1)));
// 	}else if(x<w-1){
// 		output[]x,0)=std::min(in(x,0), std::min(in(x-1,0), std::min(in(x,1), in(x+1,0))));
// 		for(unsigned y=1;y<h-1;y++){
// 			output[]x,y)=std::min(in(x,y), std::min(in(x-1,y), std::min(in(x,y-1), std::min(in(x,y+1), in(x+1,y)))));
// 		}
// 		output[]x,h-1)=std::min(in(x,h-1), std::min(in(x-1,h-1), std::min(in(x,h-2), in(x+1,h-1))));
// 	}else{
// 		output[]w-1,0)=std::min(in(w-1,0), std::min(in(w-1,1), in(w-2,0)));
// 		for(unsigned y=1;y<h-1;y++){
// 			output[]w-1,y)=std::min(in(w-1,y), std::min(in(w-1,y-1), std::min(in(w-2,y), in(w-1,y+1))));
// 		}
// 		output[]w-1,h-1)=std::min(in(w-1,h-1), std::min(in(w-1,h-2), in(w-2,h-1)));
// 	}
// }

// __kernel void erode(unsigned x, 
// 	unsigned w, 
// 	unsigned h, 
// 	__global unsigned *input, 
// 	__global unsigned *output)
// {
// 	auto in=[&](int x, int y) -> uint32_t { return input[y*w+x]; };
// 	auto out=[&](int x, int y) -> uint32_t & {return output[y*w+x]; };

// 	if(x==0){
// 		out(0,0)=std::max(in(0,0), std::max(in(0,1), in(1,0)));
// 		for(unsigned y=1;y<h-1;y++){
// 			out(0,y)=std::max(in(0,y), std::max(in(0,y-1), std::max(in(1,y), in(0,y+1))));
// 		}
// 		out(0,h-1)=std::max(in(0,h-1), std::max(in(0,h-2), in(1,h-1)));
// 	}else if(x<w-1){
// 		out(x,0)=std::max(in(x,0), std::max(in(x-1,0), std::max(in(x,1), in(x+1,0))));
// 		for(unsigned y=1;y<h-1;y++){
// 			out(x,y)=std::max(in(x,y), std::max(in(x-1,y), std::max(in(x,y-1), std::max(in(x,y+1), in(x+1,y)))));
// 		}
// 		out(x,h-1)=std::max(in(x,h-1), std::max(in(x-1,h-1), std::max(in(x,h-2), in(x+1,h-1))));
// 	}else{
// 		out(w-1,0)=std::max(in(w-1,0), std::max(in(w-1,1), in(w-2,0)));
// 		for(unsigned y=1;y<h-1;y++){
// 			out(w-1,y)=std::max(in(w-1,y), std::max(in(w-1,y-1), std::max(in(w-2,y), in(w-1,y+1))));
// 		}
// 		out(w-1,h-1)=std::max(in(w-1,h-1), std::max(in(w-1,h-2), in(w-2,h-1)));
// 	}
// }