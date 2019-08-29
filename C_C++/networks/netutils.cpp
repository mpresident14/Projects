#include "netutils.hpp"

#include <net/if.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
#include <bitset>

using namespace std;


// Iterate through interface indices
// For each index, get the name, then get its flags. If the device is up and not the 
// loopback interface, return its IP address.
in_addr_t get_my_ip()
{
	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct ifreq ipreq;
	int err;
	int index = 1;
	ipreq.ifr_ifindex = index;

	// Get device name based on index
	while ((err = ioctl(sd, SIOCGIFNAME, &ipreq)) != -1) {
		// Get device flags based on device name
		if (ioctl(sd, SIOCGIFFLAGS, &ipreq) == -1) {
			handle_error("ioctl get flags");
			continue;
		}
		uint16_t flags = ipreq.ifr_flags;
		if ((flags & IFF_UP) && !(flags & IFF_LOOPBACK)) {
			break;
		}
		ipreq.ifr_ifindex = ++index;

	}

	if (err == -1) {
		handle_error("ioctl get if name");
		return -1;
	}
	
	// Get device IP address based on device name
	if (ioctl(sd, SIOCGIFADDR, &ipreq) < 0) {
		handle_error("ioctl");
		return -1;
	}

	return ((struct sockaddr_in*) &ipreq.ifr_addr)->sin_addr.s_addr;
}

void handle_error(const char* fn)
{
	cerr << fn << ": " << strerror(errno) << endl;
}

int main()
{
	struct in_addr addr;
	addr.s_addr = get_my_ip();
	cout << inet_ntoa(addr) << endl;
}
