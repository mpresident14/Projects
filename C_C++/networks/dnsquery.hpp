#ifndef DNSQUERY_HPP_INCLUDED
#define DNSQUERY_HPP_INCLUDED 1

#include <string>
#include <vector>

std::vector<std::string> get_all_ips(const char* domain);
std::string get_ip(const char* domain);

#endif