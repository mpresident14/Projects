#include "my_traceroute.hpp"

#include <iostream>

using namespace std;

int main()
{
	MyTraceroute m;
	unique_ptr<vector<string>> vec_ptr = m.runTraceroute();
	for (auto&& iter = vec_ptr->begin(); iter != vec_ptr->end(); ++iter) {
		cout << *iter << endl;
	}
	cout << vec_ptr->size() << endl;
}
