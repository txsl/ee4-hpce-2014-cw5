__kernel void erode(unsigned x, 
	unsigned w, 
	unsigned h, 
	__global unsigned *input, 
	__global unsigned *output)
{
	if(x==0){
		output[w*(0)+0]=min(input[w*(0)+0], min(input[w*(1)+0], input[w*(0)+1]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+0]=min(input[w*(y)+0], min(input[w*(y-1)+0], min(input[w*(y)+1], input[w*(y+1)+0])));
		}
		output[w*(h-1)+0]=min(input[w*(h-1)+0], min(input[w*(h-2)+0], input[w*(h-1)+1]));
	}else if(x<w-1){
		output[w*(0)+x]=min(input[w*(0)+x], min(input[w*(0)+x-1], min(input[w*(1)+x], input[w*(0)+x+1])));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+x]=min(input[w*(y)+x], min(input[w*(y)+x-1], min(input[w*(y-1)+x], min(input[w*(y+1)+x], input[w*(y)+x+1]))));
		}
		output[w*(h-1)+x]=min(input[w*(h-1)+x], min(input[w*(h-1)+x-1], min(input[w*(h-2)+x], input[w*(h-1)+x+1])));
	}else{
		output[w*(0)+w-1]=min(input[w*(0)+w-1], min(input[w*(1)+w-1], input[w*(0)+w-2]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+w-1]=min(input[w*(y)+w-1], min(input[w*(y-1)+w-1], min(input[w*(y)+w-2], input[w*(y+1)+w-1])));
		}
		output[w*(h-1)+w-1]=min(input[w*(h-1)+w-1], min(input[w*(h-2)+w-1], input[w*(h-1)+w-2]));
	}
}

__kernel void dilate(unsigned x, 
	unsigned w, 
	unsigned h, 
	__global unsigned *input, 
	__global unsigned *output)
{
	if(x==0){
		output[w*(0)+0]=max(input[w*(0)+0], max(input[w*(1)+0], input[w*(0)+1]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+0]=max(input[w*(y)+0], max(input[w*(y-1)+0], max(input[w*(y)+1], input[w*(y+1)+0])));
		}
		output[w*(h-1)+0]=max(input[w*(h-1)+0], max(input[w*(h-2)+0], input[w*(h-1)+1]));
	}else if(x<w-1){
		output[w*(0)+x]=max(input[w*(0)+x], max(input[w*(0)+x-1], max(input[w*(1)+x], input[w*(0)+x+1])));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+x]=max(input[w*(y)+x], max(input[w*(y)+x-1], max(input[w*(y-1)+x], max(input[w*(y+1)+x], input[w*(y)+x+1]))));
		}
		output[w*(h-1)+x]=max(input[w*(h-1)+x], max(input[w*(h-1)+x-1], max(input[w*(h-2)+x], input[w*(h-1)+x+1])));
	}else{
		output[w*(0)+w-1]=max(input[w*(0)+w-1], max(input[w*(1)+w-1], input[w*(0)+w-2]));
		for(unsigned y=1;y<h-1;y++){
			output[w*(y)+w-1]=max(input[w*(y)+w-1], max(input[w*(y-1)+w-1], max(input[w*(y)+w-2], input[w*(y+1)+w-1])));
		}
		output[w*(h-1)+w-1]=max(input[w*(h-1)+w-1], max(input[w*(h-2)+w-1], input[w*(h-1)+w-2]));
	}
}


