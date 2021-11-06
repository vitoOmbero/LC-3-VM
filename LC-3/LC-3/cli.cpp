#include <iostream>
#include <fstream>

#include "LC-3.h"


int main(int argc, char** argv)
{
	argc--;
	argv++;

	if (argc < 1)
	{
		std::cout << "Only one argument is supported - the filename (object file) e.g. \"my_src.obj\"" << std::endl;
		// return EXIT_FAILURE;
	}


	//std::ifstream is(argv[0], std::ios::in | std::ios::binary);
	std::ifstream is("/../../../../Tests/hello.obj", std::ios::in | std::ios::binary);

	if (!is) {
		std::cerr << "Some problems have acquired when opening the file. Does file exist? ";
		std::cerr << argv[0];
		return EXIT_FAILURE;
	};

	if (!is.is_open()) {
		std::cerr << "Bad filename for .obj file " << argv[0] << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}