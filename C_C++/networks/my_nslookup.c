#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h> 
#include <arpa/inet.h>


/** 
 * Issues a request to the DNS server to resolve a hostname using built-in
 * operations. 
 */

/* Defined in header
struct addrinfo {
   int              ai_flags;
   int              ai_family;
   int              ai_socktype;
   int              ai_protocol;
   socklen_t        ai_addrlen;
   struct sockaddr *ai_addr;
   char            *ai_canonname;
   struct addrinfo *ai_next;
};
*/     

/* 
 * Note: multiple results because network host is accessible over multiple protocols
 * IPPROTO_UDP = 17, IPPROTO_TCP = 6
 */ 

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "Needs an argument.\n");
	}

	struct addrinfo hints;
	struct addrinfo* res;	// Linked list of struct addrinfos
	int err;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	// hints.ai_socktype = 0;
	// hints.ai_protocol = 0;
	// hints.ai_flags = 0;

	/* Runs the DNS lookup and puts results in linked list res*/
	if ((err = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
	}

	char hostname[100];

	/* Extract info from each sockaddr */
	struct sockaddr_in* sa = (struct sockaddr_in*) (res -> ai_addr);
	getnameinfo(res -> ai_addr, res -> ai_addrlen, hostname, 100, NULL, 0, 0);
	printf("hostname: %s, ip: %s, protocol: %d\n", hostname, inet_ntoa(sa -> sin_addr), res -> ai_protocol);

	struct addrinfo* current = res -> ai_next;
	while (current) {
		sa = (struct sockaddr_in*) (current -> ai_addr);
		getnameinfo(current -> ai_addr, current -> ai_addrlen, hostname, 100, NULL, 0, 0);
		printf("hostname: %s, ip: %s, protocol: %d\n", hostname, inet_ntoa(sa -> sin_addr), current -> ai_protocol);
		
		current = current -> ai_next;
	}

	freeaddrinfo(res);
	return 0;
}