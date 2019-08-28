#ifndef DNSQUERY_HPP_INCLUDED
#define DNSQUERY_HPP_INCLUDED 1

#include <string>
#include <vector>

std::vector<std::string> resolve_host(const char* domain);

#endif