#ifndef NETUTILS_HPP_INCLUDED
#define NETUTILS_HPP_INCLUDED 1

#include <netinet/ip.h>

in_addr_t get_my_ip();
void handle_error(const char* fn);

#endif