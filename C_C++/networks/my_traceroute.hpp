#ifndef MY_TRACEROUTE_HPP_INCLUDED
#define MY_TRACEROUTE_HPP_INCLUDED 1

#include <vector>
#include <memory>


struct ip_header {
  unsigned char      ip_hl:4, ip_ver:4; // Colons specify # bits
  unsigned char      ip_dscp:6, ip_ecn:2;
  unsigned short     ip_len;
  unsigned short     ip_id;
  unsigned short     ip_frag;
  unsigned char      ip_ttl;
  unsigned char      ip_protocol;
  unsigned short     ip_sum;
  unsigned int       ip_src;
  unsigned int       ip_dst;
} __attribute__((packed));

struct icmp_header {
  unsigned char icmp_type;
  unsigned char icmp_code;
  unsigned short  icmp_checksum;
  unsigned short  icmp_id;
  unsigned short  icmp_seqnum;
};

class MyTraceroute {
	public:
		std::unique_ptr<std::vector<std::string>> run_traceroute(const char* dst_ip) const;
	private:
		int ping(const char* dst_ip, unsigned port, int ttl) const;
		void fill_ip_header(struct ip_header* iphdr, const char* dst_ip, unsigned char ttl) const;
		void fill_icmp_header(struct icmp_header* icmphdr) const;
};

#endif