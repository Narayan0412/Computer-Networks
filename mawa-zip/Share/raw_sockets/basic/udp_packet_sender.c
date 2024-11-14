#define _DEFAULT_SOURCE
#include "header.h"

#define DESTMAC0 0x00
#define DESTMAC1 0x0c
#define DESTMAC2 0x29
#define DESTMAC3 0x5f
#define DESTMAC4 0x26
#define DESTMAC5 0x6a
#define INTERFACE "ens160"

unsigned short
checksum(unsigned short* buff, int _16bitword) {

	unsigned long sum;

	for (sum = 0; _16bitword > 0; _16bitword--)
		sum += htons(*(buff)++);
	sum = ((sum >> 16) + (sum & 0xFFFF));
	sum += (sum >> 16);
	
	return (unsigned short)(~sum);
}

void*
create_ethhdr(unsigned char* sendbuff, struct ifreq *ifreq_c) {

	struct ethhdr *eth = (struct ethhdr *) sendbuff ;
	eth->h_source[0] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[0]);
	eth->h_source[1] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[1]);
	eth->h_source[2] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[2]);
	eth->h_source[3] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[3]);
	eth->h_source[4] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[4]);
	eth->h_source[5] = (unsigned char) (ifreq_c->ifr_hwaddr.sa_data[5]);

	eth->h_dest[0] = DESTMAC0;
	eth->h_dest[1] = DESTMAC1;
	eth->h_dest[2] = DESTMAC2;
	eth->h_dest[3] = DESTMAC3;
	eth->h_dest[4] = DESTMAC4;
	eth->h_dest[5] = DESTMAC5;

	eth->h_proto = htons(ETH_P_IP);

	return (void*)(eth);
}

void*
create_iphdr(unsigned char* sendbuff , struct ifreq *ifreq_ip) {

	struct iphdr *iph = (struct iphdr *) (sendbuff + sizeof(struct ethhdr));
	iph->ihl = 5;
	iph->version = 4;
	iph->id = htons(10201);
	iph->ttl = 64;
	iph->protocol = 17;
	iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip->ifr_addr))->sin_addr)));
	iph->daddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip->ifr_addr))->sin_addr)));
	iph->check = checksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));
	
	return (void*)(iph);
}

void*
create_udphdr(unsigned char* sendbuff) {

	struct udphdr *uh = (struct udphdr *) (sendbuff + sizeof(struct ethhdr) + sizeof(struct iphdr));
	uh->source = htons(23451);
	uh->dest = htons(23452);
	uh->check = 0;
	
	return (void*)(uh);
}

void*
ifreq_ioctl(int sock_r, int option) {

	struct ifreq *ifreq_t = malloc(sizeof(struct ifreq));
	bzero(ifreq_t,sizeof(&ifreq_t));

	strncpy(ifreq_t->ifr_name, INTERFACE , IF_NAMESIZE-1);
	if((ioctl(sock_r,option,ifreq_t)) < 0){
		perror("ioctl error ");
		exit(EXIT_FAILURE);
	}

	return (void*)(ifreq_t);
}

void*
address(struct ifreq *ifreq_i) {

	struct sockaddr_ll *sadr_ll = malloc(sizeof(struct sockaddr_ll));
	sadr_ll->sll_ifindex = ifreq_i->ifr_ifindex;
	sadr_ll->sll_halen = ETH_ALEN;
	sadr_ll->sll_addr[0] = DESTMAC0;
	sadr_ll->sll_addr[1] = DESTMAC1;
	sadr_ll->sll_addr[2] = DESTMAC2;
	sadr_ll->sll_addr[3] = DESTMAC3;
	sadr_ll->sll_addr[4] = DESTMAC4;
	sadr_ll->sll_addr[5] = DESTMAC5;

	return (void*)(sadr_ll);
}

int
main() {

	int sock_r = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

	if(sock_r < 0) {
		perror("socket error ");
		exit(EXIT_FAILURE);
	}

	struct ifreq *ifreq_i = ifreq_ioctl(sock_r, SIOCGIFINDEX); /* INDEX */
	struct ifreq *ifreq_c = ifreq_ioctl(sock_r, SIOCGIFHWADDR); /* MAC ADDRESS */
	struct ifreq *ifreq_ip = ifreq_ioctl(sock_r, SIOCGIFADDR); /* IP */

	unsigned char* sendbuff = (unsigned char*)malloc(64);
	
	create_ethhdr(sendbuff,ifreq_c); /* ETHERNET HEADER */
	struct iphdr *iph = create_iphdr(sendbuff, ifreq_ip); /* IP HEADER */
	struct udphdr *uh = create_udphdr(sendbuff); /* UDP HEADER */
	
	int total_len = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

	// DATA
	
	sendbuff[total_len++] = 0xAA;
	sendbuff[total_len++] = 0xBB;
	sendbuff[total_len++] = 0xCC;
	sendbuff[total_len++] = 0xDD;
	sendbuff[total_len++] = 0xEE;

	// UPDATING LENGTH FIELDS OF UDP HEADER AND IP HEADER

	uh->len = htons(total_len - sizeof(struct iphdr) - sizeof(struct ethhdr));
	iph->tot_len = htons(total_len - sizeof(struct ethhdr));

	struct sockaddr_ll *sadr_ll = address(ifreq_i);	

	// SENDING
	
	while(1) {
		int send_len = sendto(sock_r,sendbuff,64,0,(struct sockaddr*)sadr_ll,sizeof(*sadr_ll));
		sleep(3);	
		if(send_len < 0) {
			perror("send error ");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

/*
 *
 *
 * raw socket
 * -> packet
 * -> UDP
 * -> IP
 */
