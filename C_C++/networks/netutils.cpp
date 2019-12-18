#include "netutils.hpp"

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>

#include <bitset>
#include <iostream>
#include <string>

using namespace std;

// Connect to arbitrary address and see which IP address the kernel uses
in_addr_t get_my_ip() {

  // Create socket
  int sd;
  if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    handle_error("socket1");
    return -1;
  }

  struct sockaddr_in dns_addr;
  dns_addr.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS addr
  dns_addr.sin_family = AF_INET;
  dns_addr.sin_port = 2353;                        // Random port

  if (connect(sd, (const sockaddr*) &dns_addr, sizeof(struct sockaddr)) < 0) {
    handle_error("connect");
    return -1;
  }

  struct sockaddr_in my_addr;
  socklen_t len;
  if (getsockname(sd, (sockaddr*) &my_addr, &len) < 0) {
    handle_error("getsockname");
    return -1;
  }

  close(sd);

  char myip_str[INET_ADDRSTRLEN];
  cout << "My IP address: " << inet_ntop(AF_INET, (const void *) &my_addr.sin_addr.s_addr, myip_str, INET_ADDRSTRLEN) << endl;
  return my_addr.sin_addr.s_addr;
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
