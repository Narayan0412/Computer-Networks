#define _DEFAULT_SOURCE 1
#include "header.h"

/*
 * PACKET SNIFFER
 * FOR TCP AND UDP PROTOCOLS
 */

void*
print_ethhdr(unsigned char* buffer) {


	struct ethhdr *eth = (struct ethhdr*) buffer;
/*	printf(" \nEthernet Header\n");
	printf(" \t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	printf(" \t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
*/
	return (void*)(eth);
}

void*
print_iphdr(unsigned char* buffer) {

	struct sockaddr_in source, dest;
	struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	bzero(&source,sizeof(source));
	source.sin_addr.s_addr = ip->saddr;
	bzero(&dest,sizeof(dest));
	dest.sin_addr.s_addr = ip->daddr;

	printf("IP Header\n");
	printf(" \t|-Version : %d\n", (unsigned int)ip->version);
	printf(" \t|-Internet Header Length : %d DWORDS or %d Bytes\n", (unsigned int)ip->ihl, 4*((unsigned int)ip->ihl));
	printf(" \t|-Type of Service : %d\n", (unsigned int)ip->tos);
	printf(" \t|-Total Length : %d\n", (unsigned int)ip->tot_len);
	printf(" \t|-Identification : %d\n", ntohs(ip->id));
	printf(" \t|-Time to Live : %d\n", (unsigned int)ip->ttl);
	printf(" \t|-Protocol : %d\n", (unsigned int)ip->protocol);
	printf(" \t|-Header Checksum : %d\n", ntohs(ip->check));
	printf(" \t|-Source IP : %s\n", inet_ntoa(source.sin_addr));
	printf(" \t|-Destination IP : %s\n", inet_ntoa(dest.sin_addr));

	return (void*)(ip);
}

void*
print_udphdr(unsigned char* buffer, unsigned short iphdrlen) {

	struct udphdr *udp = (struct udphdr*)(buffer + sizeof(struct ethhdr) + iphdrlen);

	printf("UDP Header\n");
	printf(" \t|-Source Port : %d\n", ntohs(udp->source));
	printf(" \t|-Destination Port : %d\n", ntohs(udp->dest));
	printf(" \t|-UDP Length : %d\n", ntohs(udp->len));
	printf(" \t|-UDP Checksum : %d\n", ntohs(udp->check));

	return (void*)(udp);
}

void*
print_tcphdr(unsigned char* buffer, unsigned short iphdrlen) {

   	struct tcphdr *tcp = (struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
   	printf("\nTCP Header\n");
   	printf("\t|-Source Port          : %u\n",ntohs(tcp->source));
   	printf("\t|-Destination Port     : %u\n",ntohs(tcp->dest));
   	printf("\t|-Sequence Number      : %u\n",ntohl(tcp->seq));
   	printf("\t|-Acknowledge Number   : %u\n",ntohl(tcp->ack_seq));
   	printf("\t|-Header Length        : %d DWORDS or %d BYTES\n" ,(unsigned int)tcp->doff,(unsigned int)tcp->doff*4);
	printf("\t|----------Flags-----------\n");
	printf("\t\t|-Urgent Flag          : %d\n",(unsigned int)tcp->urg);
	printf("\t\t|-Acknowledgement Flag : %d\n",(unsigned int)tcp->ack);
	printf("\t\t|-Push Flag            : %d\n",(unsigned int)tcp->psh);
	printf("\t\t|-Reset Flag           : %d\n",(unsigned int)tcp->rst);
	printf("\t\t|-Synchronise Flag     : %d\n",(unsigned int)tcp->syn);
	printf("\t\t|-Finish Flag          : %d\n",(unsigned int)tcp->fin);
	printf("\t|-Window size          : %d\n",ntohs(tcp->window));
	printf("\t|-Checksum             : %d\n",ntohs(tcp->check));
	printf("\t|-Urgent Pointer       : %d\n",tcp->urg_ptr);

	return (void*)(tcp);
}
void
print_data(unsigned char* buffer, int buf_len, unsigned short iphdrlen, int proto) {

	int tnphdr_size = 0;
	if(proto == 6)
		tnphdr_size = sizeof(struct tcphdr);
	else if(proto == 17)
		tnphdr_size = sizeof(struct udphdr);

	unsigned char* data = (buffer + iphdrlen + sizeof(struct ethhdr) + tnphdr_size);
	int remaining_data = buf_len - (iphdrlen + sizeof(struct ethhdr) + tnphdr_size);
	
	printf("Data\n");
	for(int i = 0; i < remaining_data; i++) {
		if(i != 0 && i%16 == 0)
			printf("\n");
		printf(" %.2X", data[i]);
	}
	printf("\n");

	return;
}

void
process_packet(unsigned char*  buffer, int buf_len) {

//	if (((struct iphdr*)(buffer+sizeof(struct ethhdr)))->daddr != inet_addr("192.168.60.16"))
//		return;
	print_ethhdr(buffer);
	struct iphdr *ip = (struct iphdr *) print_iphdr(buffer);
	unsigned short iphdrlen = ip->ihl * 4;

	print_payload(buffer, buf_len);
/*
	if(ip->protocol == 17) {
		print_udphdr(buffer, iphdrlen);
		print_data(buffer, buf_len, iphdrlen, ip->protocol);
	}
	else if (ip->protocol == 6) {
		print_tcphdr(buffer, iphdrlen);
		print_data(buffer, buf_len, iphdrlen, ip->protocol);
	}
*/	
	return;
}

int
main() {

	int sock_r;
	sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	
	if(sock_r < 0) {
		perror("socket error ");
		exit(EXIT_FAILURE);
	}

	unsigned char* buffer = (unsigned char*) malloc(65536);
	memset(buffer,0,sizeof(&buffer));

	struct sockaddr saddr;
	memset(&saddr,0,sizeof(saddr));
	socklen_t saddr_len = sizeof(saddr);

	while(1) {
		int buf_len = recvfrom(sock_r, buffer, 65536, 0,&saddr, &saddr_len);
		
		if(buf_len < 0) {
			perror("recvfrom error ");
			exit(EXIT_FAILURE);
		}

		process_packet(buffer, buf_len);
	}

	return 0;
}
