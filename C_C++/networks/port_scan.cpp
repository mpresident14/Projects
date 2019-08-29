#include "port_scan.hpp"

// https://en.wikipedia.org/wiki/Transmission_Control_Protocol#TCP_segment_structure
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <bitset>

#define MYIP "10.0.2.15"
#define MYPORT 24543

using namespace std;

/** 
	IDEA OF PORT SCANNING
	I send a TCP SYN packet to a port. If I get a SYN-ACK response, then I know the server
	is running an application on that port. If I get a RST response, this means the server
	is not accepting TCP connections on that port. (Note that I actually send an RST in the
	first case after receiving a SYN-ACK since I don't receive anymore packets.) If I don't
	get a response, this means the packet was blocked by a firewall.
*/

// TCP cksum is calculated over pseudoheader and TCP segment (actual header and data)
struct cksumbuf {
	// http://www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm
	struct tcp_pseudo {
		uint32_t tps_src;
		uint32_t tps_dest;
		u_char tps_reserved;
		u_char tps_protocol;
		u_short tps_length;
	} tcp_pseudo;

	struct tcphdr tcphdr;

	// Normally data included also, but SYN has no data
};

unsigned short in_cksum(unsigned short *ptr, int nbytes);
void construct_syn(struct tcphdr* tcp);

void handle_error(const char* fn)
{
	cerr << fn << ": " << strerror(errno) << endl;
}

void port_scan(const char* hostname)
{
	/* Create socket */
	// SOCK_RAW specifies I am providing the network layer datagram (i.e. the IP layer data) 
	int sd;
	if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
		return handle_error("socket");
	}

	/* Set socket options */
	// To prevent "address already in use" error */
	int reuse = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		close(sd);
		return handle_error("setsockopt SO_REUSEADDR");
	}

	// Set receive timeout
	struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
	if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		close(sd);
		return handle_error("setsockopt");
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

	const char* dst_ip{get_ip(hostname).c_str()};
  if (strlen(dst_ip) == 0) {
    cerr << "Could not resolve hostname."  << endl;
    close(sd);
    return;
  }

  cout << "Scanning " << dst_ip << endl;
  in_port_t dst_port = htons(80);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = dst_port;

	in_addr_t inet_dst_ip = inet_addr(dst_ip);
	servaddr.sin_addr.s_addr = inet_dst_ip;

	struct cksumbuf cksumbuf;

	struct cksumbuf::tcp_pseudo* tcpps = &(cksumbuf.tcp_pseudo);
	tcpps->tps_src = inet_addr(MYIP);
	tcpps->tps_dest = inet_dst_ip;
	tcpps->tps_reserved = 0;
	tcpps->tps_protocol = IPPROTO_TCP;
	// No tcp data for SYN packet, so size of segment is just the length of the header
	tcpps->tps_length = htons(sizeof(struct tcphdr));

	struct tcphdr* tcp_header = &(cksumbuf.tcphdr);
	construct_syn(tcp_header);
	struct tcphdr recv_tcphdr;
	// Only using this to validate that this is the address we sent the packet to
	struct sockaddr_in recv_sockaddr;
	socklen_t len = sizeof(struct sockaddr_in);

	for (in_port_t port = 79; port < 82; port++) {
		// Increment port and recompute checksum
		tcp_header->th_dport = htons(port);
		tcp_header->th_sum = 0;
		tcp_header->th_sum = in_cksum((unsigned short*) &cksumbuf, sizeof(cksumbuf));

		if (sendto(sd, tcp_header, sizeof(struct tcphdr), 0, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
			handle_error("sendto");
			continue;
		}
		
		if (recvfrom(sd, &recv_tcphdr, sizeof(struct tcphdr), 0, (struct sockaddr*) &recv_sockaddr, &len) < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				cout << "Destination unreachable" << endl;
			} else {
				handle_error("recvfrom");
			}
			continue;
		}

		// Validate the src addr of the received packet
		if (servaddr.sin_addr.s_addr != recv_sockaddr.sin_addr.s_addr) {
			cerr << "Invalid source address" << endl;
			continue;
		}

		cout << "ACK num=" << hex << ntohl(recv_tcphdr.th_ack) << endl;
		cout << "Port " << port << ": ";

		uint8_t flags = ntohs(recv_tcphdr.th_flags);
		if ((flags & TH_SYN) && (flags & TH_ACK)) {
			cout << "Accepting TCP" << endl;
		} else if (recv_tcphdr.th_flags & TH_RST) {
			cout << "Not listening for TCP" << endl;
		} else {
			cout << "Unexpected flags: " << bitset<8>(flags) << endl;
		}
	}

	close(sd);
}

void construct_syn(struct tcphdr* tcp)
{
	tcp->th_sport = htons((uint16_t) MYPORT);
	// Set tcp->th_dport in loop
	// Random starting sequence #
	tcp->th_seq = htonl(123);
	tcp->th_ack = 0;
	tcp->th_x2 = 0;
	tcp->th_off = 5;
	tcp->th_flags = TH_SYN;
	tcp->th_win = htons(3);
	tcp->th_sum = 0;
	tcp->th_urp = 0;
}

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

int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr << "Enter a hostname" << endl;
		return 1;
	}

	port_scan(argv[1]);
}