#include "my_traceroute.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr << "Enter a hostname." << endl;
		return 1;
	}
	
	run_traceroute(argv[1]);

	return 0;
}
