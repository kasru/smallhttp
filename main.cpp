#include "myhttp.h"

#include <iostream>

int main(int argc, char *argv[])
try
{
    std::cout << "smallhttp server 1.0" << std::endl;
	myhttp server;
	server.run();
	return EXIT_SUCCESS;
}
catch (std::exception const & ex)
{
	std::cerr << "smallhttp stop. error: " << ex.what() << std::endl;
	return EXIT_FAILURE;
}
