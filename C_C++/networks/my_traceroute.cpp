#include "my_traceroute.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
// https://en.wikipedia.org/wiki/IPv4#Packet_structure
// https://unix.superglobalmegacorp.com/Net2/newsrc/netinet/ip.h.html
// #include <netinet/ip.h>
// https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Datagram_structure
// https://www.cymru.com/Documents/ip_icmp.h
// #include <netinet/ip_icmp.h>
// #include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#include<iostream>

#define PACKET_LEN 400
#define ICMP_DATA_SIZE 56

using namespace std;

/* Compute checksum for ICMP header */
unsigned short in_cksum(unsigned short *ptr, int nbytes)
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

unique_ptr<vector<string>> MyTraceroute::run_traceroute(const char* dst_ip) const
{
  ping(dst_ip, 7, 64);
  return unique_ptr<vector<string>>(new vector<string>{"a", "b"});
}


int MyTraceroute::ping(const char* dst_ip, unsigned port, int ttl) const
{
  /* Create the socket descriptor */
  // domain: AF_INET specifies IPv4
  // type: SOCK_RAW specifies raw network protocol access
  // protocol: IPPROTO_RAW specifies raw IP packets (allows editing of IP header
  //   in addition to ICMP)
  int sd;
  if ((sd = socket(AF_INET, SOCK_RAW , IPPROTO_RAW)) < 0) {
    cerr << "Error creating socket descriptor: " << strerror(errno) << endl;
    return -1;
  }

  /* Set socket options */

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
  sin.sin_port = htons(port); // 7 for regular ping
  // inet_addr converts "a.b.c.d" to network byte order
  sin.sin_addr.s_addr = inet_addr(dst_ip);

  /* Build IP header and ICMP header */
  unsigned char buffer[PACKET_LEN] = {0};
  struct ip_header* iphdr = (struct ip_header*) buffer;
  struct icmp_header* icmphdr = (struct icmp_header*) (buffer + sizeof(struct ip_header));
  fill_ip_header(iphdr, dst_ip, ttl);
  fill_icmp_header(icmphdr);

  /* Send packet */
  if (sendto(
      sd, 
      buffer, 
      sizeof(struct ip_header) + sizeof(struct icmp_header),
      0,
      (struct sockaddr*) &sin,
      sizeof(sin)) < 0) {
    cerr << "Error sending packet: " << strerror(errno) << endl;
    return -1;
  }
  close(sd);

  /* Set up receive socket */
  int recv_sd;
  if ((recv_sd = socket(AF_INET, SOCK_RAW , IPPROTO_ICMP)) < 0) {
    cerr << "Error creating socket descriptor: " << strerror(errno) << endl;
    return -1;
  }

  /* Receive packet */
  struct sockaddr_in recv_sin;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  int recv_nbytes;
  if ((recv_nbytes = 
      recvfrom(
          recv_sd, 
          icmphdr, 
          PACKET_LEN - sizeof(struct ip_header), 
          0, 
          (struct sockaddr*) &recv_sin, 
          &addr_len)) < 0) {
    cerr << "Error receiving packet: " << strerror(errno) << endl;
    return -1;
  }
  close(recv_sd);

  /* Parse response */
  cout << "Echo reply sender: " << inet_ntoa(recv_sin.sin_addr) << endl;
  cout << "Echo reply type: " << (unsigned) icmphdr->icmp_type << endl;
  cout << "Echo reply code: " << (unsigned) icmphdr->icmp_code << endl;

  return 0;
}

void MyTraceroute::fill_ip_header(struct ip_header* iphdr, const char* dst_ip, unsigned char ttl) const
{
  iphdr->ip_ver = 4;
  iphdr->ip_hl = 5;
  iphdr->ip_dscp = 0;
  iphdr->ip_ecn = 0;
  iphdr->ip_len = htons(sizeof(struct ip_header) + sizeof(struct icmp_header));
  iphdr->ip_id = htons(44639);
  iphdr->ip_frag = 0;
  iphdr->ip_ttl = ttl;
  iphdr->ip_protocol = 1;
  iphdr->ip_sum = 0; // TODO: DO I NEED TO COMPUTE THIS ???
  inet_pton(AF_INET, "10.0.2.15", &(iphdr->ip_src)); // TODO: USE ARP LATER
  inet_pton(AF_INET, dst_ip, &(iphdr->ip_dst));

  iphdr->ip_sum = in_cksum((unsigned short *) iphdr, sizeof(struct ip_header));
}

void MyTraceroute::fill_icmp_header(struct icmp_header* icmphdr) const
{
  icmphdr->icmp_type = 8;
  icmphdr->icmp_code = 0;
  icmphdr->icmp_checksum = 0;
  icmphdr->icmp_id = htons(0x1a92);
  icmphdr->icmp_seqnum = htons(1);

  icmphdr->icmp_checksum = 
      in_cksum((unsigned short *) icmphdr, sizeof(struct icmp_header));
}
