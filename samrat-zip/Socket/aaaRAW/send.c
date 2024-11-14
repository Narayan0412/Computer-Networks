#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>


unsigned char DEST_MAC[] = {0x08, 0x00, 0x27, 0x66, 0x86, 0xf7};
unsigned char SOURC_MAC[] = {0xd0, 0x67, 0xe5, 0x12, 0x6f, 0x8f};

#define sPORT 68
#define dPORT 67

#define destination_ip "127.0.0.1"
#define sender_ip "128.0.0.5"

#define Interface "lo"


struct ifreq ifreq_c, ifreq_i, ifreq_ip;  // for each ioctl keep diffrent ifreq structure otherwise error 
                                          // may come in sending(sendto )
int sock_raw;
unsigned char *sendbuff;            //the PACKET


int total_len = 0, send_len;

void get_eth_index()								//index of interface 
{
    memset(&ifreq_i, 0, sizeof(ifreq_i));
    strncpy(ifreq_i.ifr_name, Interface, IFNAMSIZ - 1);

    if ((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0)
    {
        perror("error in index ioctl reading \n");
        exit(1);
    }

    //same as:
    //int ind=if_nametoindex("eth0");       //Interface

    printf("index=%d\n", ifreq_i.ifr_ifindex);
}

void get_mac()										//MAC add (Data link layer)
{
    memset(&ifreq_c, 0, sizeof(ifreq_c));
    strncpy(ifreq_c.ifr_name, Interface, IFNAMSIZ - 1);

    if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0)
    {
        perror("SIOCGIFHWADDR Error \n");
        exit(1);
    }

	//X-hexadecimal
	//Each number is in range from 0-255 (2^8), and there are 6, 6*8=48 that is the MAC address
    printf("Mac= %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]), (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]));


    struct ethhdr *eth = (struct ethhdr *)(sendbuff);						//local pointer to the global sendbuff

    memcpy(eth->h_source, ifreq_c.ifr_hwaddr.sa_data, ETH_ALEN);      //original
    //memcpy(eth->h_source, SOURC_MAC, ETH_ALEN);                       //spoofing
    memcpy(eth->h_dest, DEST_MAC, ETH_ALEN);
    eth->h_proto = htons(ETH_P_IP); // 0x800

    total_len += sizeof(struct ethhdr);
}

void get_data()
{
    char buf[100]="Sex sux karne ke lie tyar hojaeye Muji";
    memcpy(sendbuff+total_len, buf, strlen(buf));

    total_len+=strlen(buf);
}

void get_udp()
{
    struct udphdr *uh = (struct udphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));

    uh->source = htons(sPORT);
    uh->dest = htons(dPORT);
    uh->check = 0;

    total_len += sizeof(struct udphdr);
    printf("%d\n", total_len);
    get_data();
    printf("%d\n", total_len);
    uh->len = htons((total_len - sizeof(struct iphdr) - sizeof(struct ethhdr)));			//size of UDP header + data only
}

unsigned short checksum(unsigned short *buff, int _16bitword)
{
    unsigned long sum;
    for (sum = 0; _16bitword > 0; _16bitword--)
        sum += htons(*(buff)++);            //ntohs?
        
    do
    {
        sum = ((sum >> 16) + (sum & 0xFFFF));
    }while (sum & 0xFFFF0000);

    return (~sum);
}

void get_ip()
{
    memset(&ifreq_ip, 0, sizeof(ifreq_ip));
    strncpy(ifreq_ip.ifr_name, Interface, IFNAMSIZ - 1);
    if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0)
    {
        perror("SIOCGIFADDR Error \n");
        exit(1);
    }

    //source IP
    char myip[16];
    strcpy(myip, inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    printf("%s\n", myip);


    struct iphdr *iph = (struct iphdr *)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->id = htons(10201);
    iph->ttl = 64;
    iph->protocol = 17;     //UDP
    iph->saddr = inet_addr(myip);                //original  
    //iph->saddr= inet_addr(sender_ip);           //spofing
    iph->daddr = inet_addr(destination_ip);     // put destination IP address
    total_len += sizeof(struct iphdr);
    get_udp();

    iph->tot_len = htons(total_len - sizeof(struct ethhdr));
    iph->check = htons(checksum((unsigned short *)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr) / 2)));
}

int main()
{
    sock_raw = socket(AF_PACKET, SOCK_RAW, 16);
    //sock_raw = socket(AF_INET, SOCK_RAW, 254);
    //IPPROTO_RAW= protcol
    if (sock_raw < 0)
    {
        perror("Socket error \n");
        exit(1);
    }
    sendbuff = (unsigned char *)malloc(128); // increase in case of large data.Here data is --> AA  BB  CC  DD  EE

    get_eth_index(); // interface number
    get_mac();          
    get_ip();


	//Pcking index of interface
    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex;
    sadr_ll.sll_halen = ETH_ALEN;
    memcpy(sadr_ll.sll_addr, DEST_MAC, ETH_ALEN);

    printf("sending...\n");
    printf("%d\n", total_len);

    //printf("%d\n", sizeof(sendbuff));
    send_len = sendto(sock_raw, sendbuff, total_len, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
    printf("%c\n", sendbuff[46]);
    if (send_len < 0)
    {
        perror("error in sending....sendlen=%d....errno=%d\n");
        exit(1);
    }
    
    printf("sent!!!\n");

}
