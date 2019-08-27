#include "my_traceroute.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#include<iostream>

#define PACKET_LEN 400
#define ICMP_DATA_SIZE 56

using namespace std;

/* Compute checksum for ICMP header */
unsigned short in_cksum (unsigned short *ptr, int nbytes)
{
  long sum;		/* assumes long == 32 bits */
  u_short oddbyte;
  u_short answer;	/* assumes u_short == 16 bits */
  /*
   * Our algorithm is simple, using a 32-bit accumulator (sum),
   * we add sequential 16-bit words to it, and at the end, fold back
   * all the carry bits from the top 16 bits into the lower 16 bits.
   */
  sum = 0;
  while (nbytes > 1)
    {
      sum += *ptr++;
      nbytes -= 2;
    }
  /* mop up an odd byte, if necessary */
  if (nbytes == 1)
    {
      oddbyte = 0;		/* make sure top half is zero */
      *((u_char *) & oddbyte) = *(u_char *) ptr;	/* one byte only */
      sum += oddbyte;
    }
  /*
   * Add back carry outs from top 16 bits to low 16 bits.
   */
  sum = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
  sum += (sum >> 16);		/* add carry */
  answer = ~sum;		/* ones-complement, then truncate to 16 bits */
  return (answer);
}

unique_ptr<vector<string>> MyTraceroute::runTraceroute() const
{
  return unique_ptr<vector<string>>(new vector<string>{"a", "b"});
}

int MyTraceroute::testFn()
{
  return 4;
}
