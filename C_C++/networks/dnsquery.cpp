#include "dnsquery.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <arpa/inet.h>

#include <string>
#include <fstream>
#include <memory>
#include <vector>

using namespace std;

#define A 1

/** 
 * Issues a request to the DNS server to resolve a hostname using manual
 * socket operations. 
 */

struct dns_header {
	unsigned short id;
	unsigned short flags;
	unsigned short num_questions;
	unsigned short num_answer_rrs;
	unsigned short num_auth_rrs;
	unsigned short num_addl_rrs;
};

// Searches for first occurrence of "byte" starting at the address "start"
// Returns -1 if no occurence within "length" bytes
int findOffsetOfByte(unsigned char* start, char byte, unsigned length)
{
	unsigned i;
	for (i = 0; i < length; ++i) {
		if (start[i] == byte) {
			return i;
		}
	}
	return -1;
}

unique_ptr<string> get_dns_server()
{
	string nameserver = "nameserver ";
	ifstream infile("/etc/resolv.conf");
	unique_ptr<string> line{make_unique<string>()};
	while(getline(infile, *line)) {
		if (line->find(nameserver) == 0) {
			*line = line->substr(nameserver.size());
			printf("DNS server: %s\n", line->data());
			return line;
		}
	}

	return unique_ptr<string>(nullptr);
}

unique_ptr<vector<string>> resolve_host(const char* domain)
{
	unique_ptr<vector<string>> resolved_ips{make_unique<vector<string>>()};

	unique_ptr<string> dns_ip_str{get_dns_server()};
	if (!dns_ip_str) {
		fprintf(stderr, "Could not find DNS server.\n");
		return unique_ptr<vector<string>>(nullptr);
	}

	const char* dns_server_ip = dns_ip_str->data();
	int len = strlen(domain);

	// Extra length byte at beginning and 00 byte at the end (see below)
	// plus 2 bytes for each of Type and Class (total of 6)
	unsigned question_len = len + 6;
	unsigned char* data = new unsigned char[sizeof(struct dns_header) + question_len];

	struct dns_header* hdr = (struct dns_header*) data;
	hdr->id = htons(0x1234);
	hdr->flags = htons(0x0100);
	hdr->num_questions = htons(1);
	hdr->num_answer_rrs = 0;
	hdr->num_auth_rrs = 0;
	hdr->num_addl_rrs = 0;

	/* Parse domain name into dns question format
	 * Sections separated by dots in domain
	 * Format is ength of section + section for each section
	 * Terminate with null byte
	 * Example: www.stanford.edu -> 03 (www) 08 (stanford) 03 edu 00
	 */
	unsigned char* question = data + sizeof(struct dns_header);

	int i; // domain index
	unsigned count_index = 0; // index of current count byte
	for (i = 0; i < len; ++i) {
		if (domain[i] == '.') {
			question[count_index] = (unsigned char) (i - count_index);
			count_index = i + 1;
		}
		else {
			question[i + 1] = (unsigned char) domain[i];
		}
	}
	question[count_index] = (unsigned char) (i - count_index);
	question[i + 1] = 0;
	

	/* Type A assumed (hostname), Class IN assumed */
	unsigned short* type = (unsigned short *) (question + i + 2);
	unsigned short* dnsclass = (unsigned short *) (question + i + 4);
	*type = htons(1);
	*dnsclass = htons(1);

	/* Put DNS query as data in UDP packet and send */

	struct sockaddr_in dns_addr;
	dns_addr.sin_family = AF_INET;
	dns_addr.sin_port = htons(53); // Random port
	inet_pton(AF_INET, dns_server_ip, &(dns_addr.sin_addr)); // Convert DNS IP to network format
	

	int sd;
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	if (sendto(sd, 
				data, 
				sizeof(struct dns_header) + question_len, 
				0,
				(const struct sockaddr*) &dns_addr, 
				sizeof(struct sockaddr)) < 0) {
		perror("sendto");
		return unique_ptr<vector<string>>(nullptr);
	}

	/* *****************************
	 * Parse answer for IP(s) 
	 * Response consists of dns header, a replica of the SINGLE query, 
	 * 	and answer resource records (RRs) (and other stuff after idc about)
	 * ****************************/
	unsigned char buffer[250];
	int bytes_received = 0;
	if ((bytes_received = recvfrom(sd, buffer, 250, 0, NULL, NULL)) < 0) {
		perror("recvfrom");
		return unique_ptr<vector<string>>(nullptr);
	}

	close(sd);

	struct dns_header* response_hdr = (struct dns_header*) buffer;
	unsigned short num_answers = ntohs(response_hdr->num_answer_rrs);

	int buf_index = sizeof(struct dns_header) + question_len;
	for (i = 0; i < num_answers; ++i) {
		// Compressed names begin with bits 11, two bytes long (pointer)
		// See https://www2.cs.duke.edu/courses/fall16/compsci356/DNS/DNS-primer.pdf
		if (0xc0 & buffer[buf_index]) {
			buf_index += 2; // name
		}
		// Otherwise, name ends with a null byte
		else {
			// bytes_received - buf_index so we don't overflow
			int endOfName = 
					findOffsetOfByte(buffer + buf_index, 0, bytes_received - buf_index);
			if (endOfName == -1) {
				fprintf(stderr, "Something went wrong.\n");
			}
			buf_index += endOfName;
		}
		/* Fields in answer RR are type, class, ttl, data length, and data.
		 * We want IP addresses, which is data for type A */

		unsigned short* type = (unsigned short*) (buffer + buf_index);
		// printf("Type: %d\n", ntohs(*type));
		buf_index += 2;

		// unsigned short* dnsclass = (unsigned short*) (buffer + buf_index);
		// printf("Class: %d\n", ntohs(*dnsclass));
		buf_index += 2;

		// unsigned int* ttl = (unsigned int*) (buffer + buf_index);
		// printf("TTL: %08x\n", ntohl(*ttl));
		buf_index += 4;

		unsigned short* length = (unsigned short*) (buffer + buf_index);
		// printf("Length: %d\n", ntohs(*length));
		buf_index += 2;

		// Check if answer is type A (hostname)
		if (ntohs(*type) == A && ntohs(*length) == 4) {
			char ip_addr[INET_ADDRSTRLEN];
			// Convert ip_bytes to "a.b.c.d" ip_addr
			inet_ntop(AF_INET, buffer + buf_index, ip_addr, INET_ADDRSTRLEN);
			resolved_ips->emplace_back(ip_addr);
		}
		buf_index += ntohs(*length);		
	}

	delete[] data;
	dns_ip_str.reset();

	return resolved_ips;
}

// int main(int argc, char** argv)
// {
// 	if (argc != 2) {
// 		fprintf(stderr, "Pass host name.\n");
// 		exit(1);
// 	}

// 	unique_ptr<vector<string>> resolved_ips{resolve_host(argv[1])};
// 	if (!resolved_ips) {
// 		fprintf(stderr, "Could not resolve host name.\n");
// 		exit(1);
// 	}

// 	for (auto iter = resolved_ips->begin(); iter != resolved_ips->end(); ++iter) {
// 		printf("%s\n", iter->c_str());
// 	}
	
// 	return 0;
// }
