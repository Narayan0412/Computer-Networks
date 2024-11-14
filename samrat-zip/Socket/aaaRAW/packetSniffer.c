#include <stdio.h>
#include <linux/if_ether.h> //For ETH_P_ALL and for other
#include <net/if.h>         //For Promiscous mode i.e, monitoring mode
#include <stdlib.h>
#include <string.h>
#include <linux/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> // TCP header example
#include <netinet/udp.h> //For UDP Header

FILE *log_txt;

void error(int val, char *problem)
{
    if (val < 0)
    {
        perror(problem);
        exit(1);
    }
}

void ExtractEthhdr(unsigned char *buffer)
{
    /*
    struct ethhdr {
        unsigned char h_dest[ETH_ALEN];    // Destination eth address (6 bytes)
        unsigned char h_source[ETH_ALEN];     // Source ether address (6 bytes)
        __be16   h_proto;      // Ethernet frame type (2 bytes)
    };
    */


    struct ethhdr *eth = (struct ethhdr *)(buffer);
    fprintf(log_txt, "\nEthernet Header\n");
    fprintf(log_txt, "\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    
    
    fprintf(log_txt, "\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(log_txt, "\t|-Protocol (Type)		: %d\n", eth->h_proto);
    
}

void ExtractIPHDR(unsigned char *buffer)
{
    /*
    struct iphdr {
    #if defined(__LITTLE_ENDIAN_BITFIELD)
        __u8    ihl:4,
                version:4;
    #elif defined (__BIG_ENDIAN_BITFIELD)
        __u8    version:4,
                ihl:4;
    #else
        #error  "Please fix <asm/byteorder.h>"
    #endif
         __u8   tos;
         __u16  tot_len;
         __u16  id;
         __u16  frag_off;
         __u8   ttl;
         __u8   protocol;
         __u16  check;
         __u32  saddr;
         __u32  daddr;
         //The options start here.

    };
    */
    unsigned short iphdrlen;
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in src, dest;
    src.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr = ip->daddr;
    fprintf(log_txt, "IP HEADER \n");
    fprintf(log_txt, "\t | -Version : %d\n", (unsigned int)ip->version);
    fprintf(log_txt, "\t | -Internet Header Length : %d DWORDS or %d Bytes\n", (unsigned int)ip->ihl, ((unsigned int)(ip->ihl)) * 4);
    fprintf(log_txt, "\t | -Type Of Service : %d\n", (unsigned int)ip->tos);
    fprintf(log_txt, "\t | -Total Length : %d Bytes\n", ntohs(ip->tot_len));
    fprintf(log_txt, "\t | -Identification : %d\n", ntohs(ip->id));
    fprintf(log_txt, "\t | -Time To Live : %d\n", (unsigned int)ip->ttl);
    fprintf(log_txt, "\t | -Protocol : %d\n", (unsigned int)ip->protocol);
    fprintf(log_txt, "\t | -Header Checksum : %d\n", ntohs(ip->check));
    fprintf(log_txt, "\t | -Source IP : %s\n", inet_ntoa(src.sin_addr));
    fprintf(log_txt, "\t | -Destination IP : %s\n", inet_ntoa(dest.sin_addr));
}

void payload(unsigned char *buffer, int buflen)
{
    //struct iphdr *ip = (struct iphdr *)(buffer);
    
    //printf("%s\n",(buffer+(ip->ihl)*4));
    unsigned char *data = (buffer + sizeof(struct iphdr) + sizeof(struct ethhdr) + sizeof(struct udphdr));
    printf("%s\n", data);

    fprintf(log_txt, "\nData\n");
    fprintf(log_txt, "%s ", data);

    // int remaining_data = buflen - (sizeof(struct iphdr) + sizeof(struct ethhdr) + sizeof(struct udphdr));

    // for (int i = 0; i < remaining_data; i++)
    // {
    //     if (i != 0 && i % 16 == 0)
    //         fprintf(log_txt, "\n");
    //     fprintf(log_txt, " %.2X ", data[i]);
    // }

    fprintf(log_txt, "\n");
}

void ExtractUDP(unsigned char *buffer)
{
	/*
    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
     getting actual size of IP header as ihl stores length in short which in 2 bytes
    int iphdrlen = ip->ihl * 4;
    getting pointer to udp header
    struct udphdr *udp = (struct udphdr *)(buffer + iphdrlen + sizeof(struct ethhdr));
    */
    
    struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));

    

    // Writing the Ethernet and IP header to the log_txt
    fprintf(log_txt, "UDP PACKET\n");
    ExtractEthhdr(buffer);
    ExtractIPHDR(buffer);
    // Writing the UPD header to the log_txt
    fprintf(log_txt, "UDP HEADER\n");
    fprintf(log_txt, "\t|-Source Port : %d\n", ntohs(udp->source));
    fprintf(log_txt, "\t|-Destination Port : %d\n", ntohs(udp->dest));
    fprintf(log_txt, "\t|-UDP Length : %d\n", ntohs(udp->len));
    fprintf(log_txt, "\t|-UDP Checksum : %d\n", ntohs(udp->check));
    
    payload(buffer, sizeof(buffer));
    fprintf(log_txt, "********* UDP PACKET ENDED *********\n");
}

void ExtractTCP(unsigned char *buffer)
{

    fprintf(log_txt, "\nTCP Packet\n");
    ExtractEthhdr(buffer);
    ExtractIPHDR(buffer);

    struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct iphdr) + sizeof(struct ethhdr));
    fprintf(log_txt, "\nTCP header\n");
    fprintf(log_txt, "\t|-Source Port          : %u\n", ntohs(tcp->source));
    fprintf(log_txt, "\t|-Destination Port     : %u\n", ntohs(tcp->dest));
    fprintf(log_txt, "\t|-Sequence Number      : %u\n", ntohl(tcp->seq));
    fprintf(log_txt, "\t|-Acknowledge Number   : %u\n", ntohl(tcp->ack_seq));
    fprintf(log_txt, "\t|-Header Length        : %d DWORDS or %d BYTES\n", (unsigned int)tcp->doff, (unsigned int)tcp->doff * 4);
    fprintf(log_txt, "\t|----------Flags-----------\n");
    fprintf(log_txt, "\t\t|-Urgent Flag          : %d\n", (unsigned int)tcp->urg);
    fprintf(log_txt, "\t\t|-Acknowledgement Flag : %d\n", (unsigned int)tcp->ack);
    fprintf(log_txt, "\t\t|-Push Flag            : %d\n", (unsigned int)tcp->psh);
    fprintf(log_txt, "\t\t|-Reset Flag           : %d\n", (unsigned int)tcp->rst);
    fprintf(log_txt, "\t\t|-Synchronise Flag     : %d\n", (unsigned int)tcp->syn);
    fprintf(log_txt, "\t\t|-Finish Flag          : %d\n", (unsigned int)tcp->fin);
    fprintf(log_txt, "\t|-Window size          : %d\n", ntohs(tcp->window));
    fprintf(log_txt, "\t|-Checksum             : %d\n", ntohs(tcp->check));
    fprintf(log_txt, "\t|-Urgent Pointer       : %d\n", tcp->urg_ptr);

    payload(buffer, sizeof(buffer));
    fprintf(log_txt, "********************************TCP PACKET ENDED*********************************\n\n\n");
}

int main()
{
    int raw_socket;
    struct sockaddr_in source_addr;
    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    unsigned char *buffer = (unsigned char *)malloc(65536); // to receive data
    

    log_txt = fopen("log.txt", "w");
    if (!log_txt)
    {
        perror("failed to open log.txt");
        exit(1);
    }

    
    int sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    //int sock_r = socket(AF_INET, SOCK_RAW, 254);
    error(sock_r, "socket Error \n");


    int tcp = 0, udp = 0, other = 0, total = 0;
    while (1)
    {
    	memset(buffer, 0, 65536);
        struct sockaddr saddr;
        int saddr_len = sizeof(saddr);
        int bufflen = recvfrom(sock_r, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
        error(bufflen, "receive from error\n");

        fflush(log_txt);

        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        ++total;
        printf("%d\n", ip->protocol);

        switch (ip->protocol)
        {
            case 6:
                ++tcp;
                ExtractTCP(buffer);
                break;

            case 17:
                ++udp;
                ExtractUDP(buffer);
                break;

            default:
                ++other;
        }
        printf("TCP: %d  UDP: %d  Other: %d  Toatl: %d  \n", tcp, udp, other, total);
        printf("%d\n", bufflen);
    }
    close(raw_socket);
    printf("Finished !!! \n");

    return 0;
}

