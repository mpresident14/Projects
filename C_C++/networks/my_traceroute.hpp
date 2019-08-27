#ifndef MY_TRACEROUTE_HPP_INCLUDED
#define MY_TRACEROUTE_HPP_INCLUDED 1

#include <vector>
#include <memory>
// https://en.wikipedia.org/wiki/IPv4#Packet_structure
// https://unix.superglobalmegacorp.com/Net2/newsrc/netinet/ip.h.html
#include <netinet/ip.h>
// https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Datagram_structure
// https://www.cymru.com/Documents/ip_icmp.h
#include <netinet/ip_icmp.h>

struct ip_icmp {
	struct ip ip_header;
	struct icmphdr icmp_header;
};

void run_traceroute(const char* dst_ip);

#endif