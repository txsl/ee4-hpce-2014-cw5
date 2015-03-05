__kernel void erode(
	__global unsigned *input, 
	__global unsigned *output)
{
	uint x = get_global_id(0);
	uint y = get_global_id(1);

	uint w = get_global_size(0);
	uint h = get_global_size(1);

	uint index = y*w+x;

	uint left  = (x != 0) ? input[index-1] : input[index];
	uint up    = (y != 0) ? input[index-w] : input[index];
	uint right = (x != w-1) ? input[index+1] : input[index];
	uint down  = (y != h-1) ? input[index+w] : input[index];

	output[index] = min(input[index], min(left, min(up, min(right, down))));
}

__kernel void dilate(
	__global unsigned *input, 
	__global unsigned *output)
{
	uint x = get_global_id(0);
	uint y = get_global_id(1);

	uint w = get_global_size(0);
	uint h = get_global_size(1);

	uint index = y*w+x;

	uint left  = (x != 0) ? input[index-1] : input[index];
	uint up    = (y != 0) ? input[index-w] : input[index];
	uint right = (x != w-1) ? input[index+1] : input[index];
	uint down  = (y != h-1) ? input[index+w] : input[index];

	output[index] = max(input[index], max(left, max(up, max(right, down))));
}