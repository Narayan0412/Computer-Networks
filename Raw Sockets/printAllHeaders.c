#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>
#include <unistd.h>

void *process_tcp_packets(void *arg);
void *process_udp_packets(void *arg);
void *process_icmp_packets(void *arg);
void *process_raw_packets(void *arg);

void process_packet(unsigned char *buffer, int size);
void print_ip_header(unsigned char *buffer, int size);
void print_tcp_packet(unsigned char *buffer, int size);
void print_udp_packet(unsigned char *buffer, int size);
void print_icmp_packet(unsigned char *buffer, int size);

// Buffer for receiving packets
unsigned char buffer[65536];

int main() {
    pthread_t tcp_thread, udp_thread, icmp_thread, raw_thread;

    // Create threads for each protocol
    pthread_create(&tcp_thread, NULL, process_tcp_packets, NULL);
    pthread_create(&udp_thread, NULL, process_udp_packets, NULL);
    pthread_create(&icmp_thread, NULL, process_icmp_packets, NULL);
    pthread_create(&raw_thread, NULL, process_raw_packets, NULL);

    // Wait for the threads to finish (this never happens in this example)
    pthread_join(tcp_thread, NULL);
    pthread_join(udp_thread, NULL);
    pthread_join(icmp_thread, NULL);
    pthread_join(raw_thread, NULL);

    return 0;
}

// Function to process TCP packets in a separate thread
void *process_tcp_packets(void *arg) {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    // Create a raw socket for TCP
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        perror("TCP Socket Error");
        return NULL;
    }

    while (1) {
        // Receive TCP packet
        int data_size = recvfrom(sock_raw, buffer, sizeof(buffer), 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom TCP error");
            break;
        }
        process_packet(buffer, data_size);
    }

    close(sock_raw);
    return NULL;
}

// Function to process UDP packets in a separate thread
void *process_udp_packets(void *arg) {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    // Create a raw socket for UDP
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw < 0) {
        perror("UDP Socket Error");
        return NULL;
    }

    while (1) {
        // Receive UDP packet
        int data_size = recvfrom(sock_raw, buffer, sizeof(buffer), 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom UDP error");
            break;
        }
        process_packet(buffer, data_size);
    }

    close(sock_raw);
    return NULL;
}

// Function to process ICMP packets in a separate thread
void *process_icmp_packets(void *arg) {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    // Create a raw socket for ICMP
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_raw < 0) {
        perror("ICMP Socket Error");
        return NULL;
    }

    while (1) {
        // Receive ICMP packet
        int data_size = recvfrom(sock_raw, buffer, sizeof(buffer), 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom ICMP error");
            break;
        }
        process_packet(buffer, data_size);
    }

    close(sock_raw);
    return NULL;
}

// Function to process RAW packets in a separate thread
void *process_raw_packets(void *arg) {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    // Create a raw socket for RAW IP packets
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock_raw < 0) {
        perror("RAW Socket Error");
        return NULL;
    }

    while (1) {
        // Receive RAW IP packet
        int data_size = recvfrom(sock_raw, buffer, sizeof(buffer), 0, &saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom RAW error");
            break;
        }
        process_packet(buffer, data_size);
    }

    close(sock_raw);
    return NULL;
}

// Process a captured packet based on its protocol
void process_packet(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;

    switch (iph->protocol) {
        case IPPROTO_TCP:
            printf("\nTCP Packet:\n");
            print_tcp_packet(buffer, size);
            break;

        case IPPROTO_UDP:
            printf("\nUDP Packet:\n");
            print_udp_packet(buffer, size);
            break;

        case IPPROTO_ICMP:
            printf("\nICMP Packet:\n");
            print_icmp_packet(buffer, size);
            break;

        case IPPROTO_RAW:
            printf("\nRAW Packet:\n");
            print_ip_header(buffer, size);
            break;

        default:
            printf("\nOther Protocol: %d\n", iph->protocol);
            print_ip_header(buffer, size);
            break;
    }
}

// Print IP Header
void print_ip_header(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;
    struct sockaddr_in source, dest;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    printf("IP Header:\n");
    printf("   |-IP Version        : %d\n", (unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
    printf("   |-Type Of Service   : %d\n", (unsigned int)iph->tos);
    printf("   |-Total Length      : %d Bytes\n", ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n", ntohs(iph->id));
    printf("   |-TTL               : %d\n", (unsigned int)iph->ttl);
    printf("   |-Protocol          : %d\n", (unsigned int)iph->protocol);
    printf("   |-Checksum          : %d\n", ntohs(iph->check));
    printf("   |-Source IP         : %s\n", inet_ntoa(source.sin_addr));
    printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
}

// Print TCP Packet
void print_tcp_packet(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;
    unsigned short iphdrlen = iph->ihl * 4;

    struct tcphdr *tcph = (struct tcphdr *)(buffer + iphdrlen);

    printf("TCP Header:\n");
    printf("   |-Source Port      : %u\n", ntohs(tcph->source));
    printf("   |-Destination Port : %u\n", ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n", ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n", ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d DWORDS or %d Bytes\n", (unsigned int)tcph->doff, (unsigned int)tcph->doff * 4);
    printf("   |-Urgent Flag          : %d\n", (unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n", (unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n", (unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n", (unsigned int)tcph->rst);
    printf("   |-Synchronise Flag     : %d\n", (unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n", (unsigned int)tcph->fin);
    printf("   |-Window         : %d\n", ntohs(tcph->window));
    printf("   |-Checksum       : %d\n", ntohs(tcph->check));
    printf("   |-Urgent Pointer : %d\n", tcph->urg_ptr);
}

// Print UDP Packet
void print_udp_packet(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;
    unsigned short iphdrlen = iph->ihl * 4;

    struct udphdr *udph = (struct udphdr *)(buffer + iphdrlen);

    printf("UDP Header:\n");
    printf("   |-Source Port      : %d\n", ntohs(udph->source));
    printf("   |-Destination Port : %d\n", ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n", ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n", ntohs(udph->check));
}

// Print ICMP Packet
void print_icmp_packet(unsigned char *buffer, int size) {
    struct iphdr *iph = (struct iphdr *)buffer;
    unsigned short iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdrlen);

    printf("ICMP Header:\n");
    printf("   |-Type       : %d\n", (unsigned int)(icmph->type));
    printf("   |-Code       : %d\n", (unsigned int)(icmph->code));
    printf("   |-Checksum   : %d\n", ntohs(icmph->checksum));
}
