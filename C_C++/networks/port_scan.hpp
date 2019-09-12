#ifndef PORT_SCAN_HPP
#define PORT_SCAN_HPP 1

#include "dnsquery.hpp"

void port_scan(const char* hostname, u_short startport, u_short endport);

#endif
