#ifndef DNS_QUERY_HPP_INCLUDED
#define DNS_QUERY_HPP_INCLUDED 1

#include <string>
#include <memory>
#include <vector>

std::unique_ptr<std::vector<std::string>> resolve_host(const char* domain);

#endif