#include "netutils.hpp"

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <bitset>
#include <iostream>
#include <string>

using namespace std;

// Iterate through interface indices
// For each index, get the name, then get its flags. If the device is up and not
// the loopback interface, return its IP address.
in_addr_t get_my_ip() {
  int sd;
  if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    handle_error("socket");
    return -1;
  }
  struct ifreq ipreq;
  int err;
  int index = 1;
  ipreq.ifr_ifindex = index;

  // Get device name based on index
  while ((err = ioctl(sd, SIOCGIFNAME, &ipreq)) != -1) {
    // Get device flags based on device name
    if (ioctl(sd, SIOCGIFFLAGS, &ipreq) == -1) {
      handle_error("ioctl get flags");
      close(sd);
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
    close(sd);
    return -1;
  }

  // Get device IP address based on device name
  if (ioctl(sd, SIOCGIFADDR, &ipreq) < 0) {
    handle_error("ioctl");
    close(sd);
    return -1;
  }

  close(sd);
  return ((struct sockaddr_in *)&ipreq.ifr_addr)->sin_addr.s_addr;
}

void handle_error(const char *fn) {
  cerr << fn << ": " << strerror(errno) << endl;
}

/* http://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/unpv12e/libfree/in_cksum.c
 */
unsigned short in_cksum(unsigned short *ptr, int nbytes) {
  long sum; /* assumes long == 32 bits */
  u_short oddbyte;
  u_short answer; /* assumes u_short == 16 bits */
  /*
   * Our algorithm is simple, using a 32-bit accumulator (sum),
   * we add sequential 16-bit words to it, and at the end, fold back
   * all the carry bits from the top 16 bits into the lower 16 bits.
   */
  sum = 0;
  while (nbytes > 1) {
    sum += *ptr++;
    nbytes -= 2;
  }
  /* mop up an odd byte, if necessary */
  if (nbytes == 1) {
    oddbyte = 0;                            /* make sure top half is zero */
    *((u_char *)&oddbyte) = *(u_char *)ptr; /* one byte only */
    sum += oddbyte;
  }
  /*
   * Add back carry outs from top 16 bits to low 16 bits.
   */
  sum = (sum >> 16) + (sum & 0xffff); /* add high-16 to low-16 */
  sum += (sum >> 16);                 /* add carry */
  answer = ~sum; /* ones-complement, then truncate to 16 bits */
  return (answer);
}
