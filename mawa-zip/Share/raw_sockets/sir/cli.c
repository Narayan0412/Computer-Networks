#define _DEFAULT_SOURCE
#include "../basic/header.h"

void*
print_iphdr(unsigned char* buffer) {

	struct sockaddr_in source, dest;
	struct iphdr *ip = (struct iphdr*)(buffer);
	bzero(&source,sizeof(source));
	source.sin_addr.s_addr = ip->saddr;
	bzero(&dest,sizeof(dest));
	dest.sin_addr.s_addr = ip->daddr;

	printf("\nIP Header\n");
	printf(" \t|-Version : %d\n", (unsigned int)ip->version);
	printf(" \t|-Internet Header Length : %d DWORDS or %d Bytes\n", (unsigned int)ip->ihl, 4*((unsigned int)ip->ihl));
	printf(" \t|-Type of Service : %d\n", (unsigned int)ip->tos);
	printf(" \t|-Total Length : %d\n", (unsigned int)ip->tot_len);
	printf(" \t|-Identification : %d\n", ntohs(ip->id));
	printf(" \t|-Time to Live : %d\n", (unsigned int)ip->ttl);
	printf(" \t|-Protocol : %d\n", (unsigned int)ip->protocol);
	printf(" \t|-Header Checksum : %d\n", ntohs(ip->check));
	printf(" \t|-Source IP : %s\n", inet_ntoa(*(struct in_addr *)&ip->saddr));
	printf(" \t|-Destination IP : %s\n", inet_ntoa(dest.sin_addr));

	return (void*)(ip);
}

int
main() {

	int rsfd = socket(AF_INET, SOCK_RAW, 254);
	if (rsfd < 0) {
		perror("socket() error ");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	socklen_t socklen = sizeof(addr);

	unsigned char* data = (unsigned char*) malloc(sizeof(struct iphdr));	

	if (recvfrom(rsfd, data, sizeof(struct iphdr), 0, (struct sockaddr*)&addr, &socklen) < 0) {
		perror("recvfrom error ");
		exit(EXIT_FAILURE);
	}
	printf("\nRAW DATA :\n\n");

	print_payload(data,sizeof(struct iphdr));
	print_iphdr(data);

	close(rsfd);

	return 0;
}
