#include "dnsquery.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Enter a hostname." << endl;
    return 1;
  }

  vector<string> ips{get_all_ips(argv[1])};

  for (auto& ip : ips) {
    cout << ip << endl;
  }

  return 0;
}
