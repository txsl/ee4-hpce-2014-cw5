#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>

#include "utilities/hpceutils.h"
#include "algorithms/standard.h"
#include "algorithms/optimised.h"


#if !(defined(_WIN32) || defined(_WIN64))
	void set_binary_io()
	{}
#else
	void set_binary_io()
	{
		_setmode(_fileno(stdin), _O_BINARY);
		_setmode(_fileno(stdout), _O_BINARY);
	}
#endif


int main(int argc, char *argv[])
{
	try{

		unsigned w, h, bits;
		int levels;
		hpce::utils::validate(argc, argv, w, h, bits, levels);
		
		
		fprintf(stderr, "Processing %d x %d image with %d bits per pixel.\n", w, h, bits);
		set_binary_io();
		
		return hpce::optimised::go(w, h, bits, levels);


	}catch(std::exception &e){
		std::cerr<<"Caught exception : "<<e.what()<<"\n";
		return 1;
	}
}

