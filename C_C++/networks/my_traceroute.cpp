#include "my_traceroute.hpp"
#include "netutils.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
// https://en.wikipedia.org/wiki/IPv4#Packet_structure
// https://unix.superglobalmegacorp.com/Net2/newsrc/netinet/ip.h.html
#include <netinet/ip.h>
// https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Datagram_structure
// https://www.cymru.com/Documents/ip_icmp.h
#include <netinet/ip_icmp.h>

#include <iostream>

#define IP_HDR_SIZE sizeof(struct ip)
#define ICMP_HDR_SIZE sizeof(struct icmphdr)
#define MAX_TTL 64
#define TIMEOUT_SEC 5

using namespace std;

struct ip_icmp {
  struct ip ip_header;
  struct icmphdr icmp_header;
};

int ping(int sd, struct sockaddr_in& sin, struct sockaddr_in& recv_sin,
         struct icmphdr& send_packet, struct ip_icmp& recv_packet, int ttl);
void fill_icmp_header(struct icmphdr* icmp_header);

void run_traceroute(const char* server) {
  /* Configure sockaddr strutures with destination. ICMP does not have a port.
   */
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
  // ICMP has no concept of port
  sin.sin_port = 0;

  // Try to convert to network byte IP. If it cannot, assume it is a hostname
  // and try to resolve it.
  const char* dst_ip;
  if (inet_pton(AF_INET, server, &(sin.sin_addr)) != 1) {
    // inet_addr converts "a.b.c.d" to network byte order
    dst_ip = get_ip(server).c_str();
    if (strlen(dst_ip) == 0) {
      cerr << "Could not resolve hostname." << endl;
      return;
    }
    inet_pton(AF_INET, dst_ip, &(sin.sin_addr));
  } else {
    dst_ip = server;
  }
  cout << "Finding route to " << dst_ip << '\n' << endl;

  /* Create the socket descriptor */

  // domain: AF_INET specifies IPv4
  // type: SOCK_RAW specifies raw network protocol access
  // protocol: IPPROTO_ICMP specifies that we are only providing the ICMP part.
  //   The OS will provide the IP header. (We can provide our own IP header
  //   using IPPROTO_RAW, but we cannot receive with this socket.)
  int sd;
  if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    cerr << "Error creating socket descriptor: " << strerror(errno) << endl;
    return;
  }

  // Set receive timeout
  struct timeval recv_timeout;
  recv_timeout.tv_sec = TIMEOUT_SEC;
  recv_timeout.tv_usec = 0;
  if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                 sizeof(struct timeval)) < 0) {
    cerr << "Error setting socket options: " << strerror(errno) << endl;
    close(sd);
    return;
  }

  sin.sin_addr.s_addr = inet_addr(dst_ip);
  struct sockaddr_in recv_sin;

  /* Build ICMP packet */
  struct icmphdr send_packet;
  fill_icmp_header(&send_packet);
  struct ip_icmp recv_packet;

  unsigned char ttl = 1;
  while (ttl <= MAX_TTL && ping(sd, sin, recv_sin, send_packet, recv_packet,
                                ttl) != ICMP_ECHOREPLY) {
    ++ttl;
  }

  if (ttl > MAX_TTL) {
    cerr << "Could not find " << server << " (" << dst_ip << ")" << endl;
  }

  close(sd);
}

int ping(int sd, struct sockaddr_in& sin, struct sockaddr_in& recv_sin,
         struct icmphdr& send_packet, struct ip_icmp& recv_packet, int ttl) {
  /* Set socket options */
  // Set time-to-live in the IP header
  if (setsockopt(sd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
    cerr << "Error setting socket options: " << strerror(errno) << endl;
    return -1;
  }

  /*
   * Send packet. We only need to send the ICMP part since we specified
   * IPPROTO_ICMP during socket creation.
   */
  if (sendto(sd, &send_packet, ICMP_HDR_SIZE, 0, (struct sockaddr*)&sin,
             sizeof(sin)) < 0) {
    cerr << "Error sending packet: " << strerror(errno) << endl;
    return -1;
  }

  /* Receive packet. We receive both the IP and ICMP headers. */
  socklen_t addr_len = sizeof(struct sockaddr_in);
  int recv_nbytes;
  if ((recv_nbytes = recvfrom(sd, &recv_packet, IP_HDR_SIZE + ICMP_HDR_SIZE, 0,
                              (struct sockaddr*)&recv_sin, &addr_len)) < 0) {
    if (errno == EWOULDBLOCK || recv_nbytes == EAGAIN) {
      cout << "Request timed out." << endl;
    } else {
      cerr << "Error receiving packet: " << strerror(errno) << endl;
    }
    return -1;
  }

  /* Parse response */
  // cout << "Echo reply sender: " << inet_ntoa(recv_sin.sin_addr) << endl;
  // cout << "Echo reply type: " << (unsigned) icmp_header.type << endl;
  // cout << "Echo reply code: " << (unsigned) icmp_header.code << endl;
  cout << inet_ntoa(recv_sin.sin_addr) << endl;

  return recv_packet.icmp_header.type;
}

void fill_icmp_header(struct icmphdr* icmp_header) {
  icmp_header->type = ICMP_ECHO;
  icmp_header->code = 0;
  icmp_header->checksum = 0;
  icmp_header->un.echo.id = getpid();
  icmp_header->un.echo.sequence = htons(1);

  icmp_header->checksum = in_cksum((unsigned short*)icmp_header, ICMP_HDR_SIZE);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Enter a hostname." << endl;
    return 1;
  }

  run_traceroute(argv[1]);

  return 0;
}
