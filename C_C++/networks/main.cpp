#include "my_traceroute.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr << "Enter an IP address" << endl;
		return 1;
	}
	MyTraceroute m;
	unique_ptr<vector<string>> vec_ptr = m.run_traceroute(argv[1]);
	for (auto&& iter = vec_ptr->begin(); iter != vec_ptr->end(); ++iter) {
		cout << *iter << endl;
	}
	cout << vec_ptr->size() << endl;
	
	return 0;
}
