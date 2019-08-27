#include "my_traceroute.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>

#define IP_HDR_SIZE sizeof(struct ip)
#define ICMP_HDR_SIZE sizeof(struct icmphdr)

using namespace std;

int ping(
    const char* dst_ip,
    int sd,
    struct sockaddr_in& sin,
    struct sockaddr_in& recv_sin,
    struct icmphdr& send_packet,
    struct ip_icmp& recv_packet,
    int ttl);
void fill_icmp_header(struct icmphdr* icmp_header);
unsigned short in_cksum(unsigned short *ptr, int nbytes);

void run_traceroute(const char* dst_ip)
{
  /* Create the socket descriptor */

  // domain: AF_INET specifies IPv4
  // type: SOCK_RAW specifies raw network protocol access
  // protocol: IPPROTO_ICMP specifies that we are only providing the ICMP part.
  //   The OS will provide the IP header. (We can provide our own IP header using
  //   IPPROTO_RAW, but we cannot receive with this socket.)
  int sd;
  if ((sd = socket(AF_INET, SOCK_RAW , IPPROTO_ICMP)) < 0) {
    cerr << "Error creating socket descriptor: " << strerror(errno) << endl;
    return;
  }

  /* Configure sockaddr strutures with destination. ICMP does not have a port. */
  // struct sockaddr_in {
  //   sa_family_t    sin_family; /* address family: AF_INET */
  //   in_port_t      sin_port;   /* port in network byte order */
  //   struct in_addr sin_addr;   /* internet address */
  // };

  // struct in_addr {
  //   uint32_t       s_addr;     /* address in network byte order */
  // };

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  // inet_addr converts "a.b.c.d" to network byte order
  sin.sin_addr.s_addr = inet_addr(dst_ip);
  struct sockaddr_in recv_sin;

  /* Build ICMP packet */
  struct icmphdr send_packet;
  fill_icmp_header(&send_packet);
  struct ip_icmp recv_packet;

  unsigned char ttl = 1;
  while (ttl <= 64 && 
      ping(dst_ip, sd, sin, recv_sin, send_packet, recv_packet, ttl) != ICMP_ECHOREPLY) {
    ++ttl;
  }

  if (ttl > 64) {
    cerr << "Could not find " << dst_ip << endl;
  }

  close(sd);
}


int ping(
    const char* dst_ip,
    int sd,
    struct sockaddr_in& sin,
    struct sockaddr_in& recv_sin,
    struct icmphdr& send_packet,
    struct ip_icmp& recv_packet,
    int ttl)
{
  /* Set socket options */
  // Set time-to-live in the IP header
  if (setsockopt(sd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
    cerr << "Error setting socket options: " << strerror(errno) << endl;
    return -1;
  }  

  /* 
   * Send packet. We only need to send the ICMP part since we specified IPPROTO_ICMP 
   * during socket creation. 
   */
  if (sendto(
      sd, 
      &send_packet,
      ICMP_HDR_SIZE,
      0,
      (struct sockaddr*) &sin,
      sizeof(sin)) < 0) {
    cerr << "Error sending packet: " << strerror(errno) << endl;
    return -1;
  }

  /* Receive packet. We receive both the IP and ICMP headers. */
  socklen_t addr_len = sizeof(struct sockaddr_in);
  int recv_nbytes;
  if ((recv_nbytes = 
      recvfrom(
          sd, 
          &recv_packet,
          IP_HDR_SIZE + ICMP_HDR_SIZE, 
          0, 
          (struct sockaddr*) &recv_sin, 
          &addr_len)) < 0) {
    cerr << "Error receiving packet: " << strerror(errno) << endl;
    return -1;
  }

  /* Parse response */
  // cout << "Echo reply sender: " << inet_ntoa(recv_sin.sin_addr) << endl;
  // cout << "Echo reply type: " << (unsigned) icmp_header.type << endl;
  // cout << "Echo reply code: " << (unsigned) icmp_header.code << endl;
  cout << inet_ntoa(recv_sin.sin_addr) << endl;

  return recv_packet.icmp_header.type;
}

void fill_icmp_header(struct icmphdr* icmp_header)
{
  icmp_header->type = ICMP_ECHO;
  icmp_header->code = 0;
  icmp_header->checksum = 0;
  icmp_header->un.echo.id = getpid();
  icmp_header->un.echo.sequence = htons(1);

  icmp_header->checksum = 
      in_cksum((unsigned short *) icmp_header, ICMP_HDR_SIZE);
}

/* Compute checksum for ICMP header */
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
  long sum;   /* assumes long == 32 bits */
  u_short oddbyte;
  u_short answer; /* assumes u_short == 16 bits */
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
      oddbyte = 0;    /* make sure top half is zero */
      *((u_char *) & oddbyte) = *(u_char *) ptr;  /* one byte only */
      sum += oddbyte;
    }
  /*
   * Add back carry outs from top 16 bits to low 16 bits.
   */
  sum = (sum >> 16) + (sum & 0xffff); /* add high-16 to low-16 */
  sum += (sum >> 16);   /* add carry */
  answer = ~sum;    /* ones-complement, then truncate to 16 bits */
  return (answer);
}
