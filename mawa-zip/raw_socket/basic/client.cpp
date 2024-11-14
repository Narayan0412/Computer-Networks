#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 65536

// Pseudo header needed for TCP checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// Function to calculate the checksum (TCP checksum)
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }

    if (len == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to create raw socket and send a packet
void start_client() {
    int sock_raw;
    struct sockaddr_in dest_info;
    struct iphdr *ip_header = (struct iphdr *)malloc(sizeof(struct iphdr));
    struct tcphdr *tcp_header = (struct tcphdr *)malloc(sizeof(struct tcphdr));
    unsigned char *packet = (unsigned char *)malloc(BUFFER_SIZE);

    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Socket creation failed!\n";
        return;
    }

    dest_info.sin_family = AF_INET;
    dest_info.sin_port = htons(80);
    dest_info.sin_addr.s_addr = inet_addr("192.168.1.100"); // Replace with server's IP

    memset(packet, 0, BUFFER_SIZE);

    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    ip_header->id = htonl(54321); // Random ID
    ip_header->frag_off = 0;
    ip_header->ttl = 255;
    ip_header->protocol = IPPROTO_TCP;
    ip_header->check = 0; // Leave checksum 0 now, filled by pseudo header
    ip_header->saddr = inet_addr("192.168.1.1"); // Replace with source IP
    ip_header->daddr = dest_info.sin_addr.s_addr;

    tcp_header->source = htons(12345);
    tcp_header->dest = htons(80);
    tcp_header->seq = 0;
    tcp_header->ack_seq = 0;
    tcp_header->doff = 5; // TCP header size
    tcp_header->fin = 0;
    tcp_header->syn = 1;
    tcp_header->rst = 0;
    tcp_header->psh = 0;
    tcp_header->ack = 0;
    tcp_header->urg = 0;
    tcp_header->window = htons(5840); /* maximum allowed window size */
    tcp_header->check = 0; // Leave checksum 0 now, filled by pseudo header
    tcp_header->urg_ptr = 0;

    // Now the TCP checksum
    struct pseudo_header psh;
    psh.source_address = inet_addr("192.168.1.1"); // Replace with source IP
    psh.dest_address = dest_info.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
    unsigned char *pseudogram = (unsigned char *)malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcp_header, sizeof(struct tcphdr));

    tcp_header->check = checksum((unsigned short *)pseudogram, psize);

    memcpy(packet, ip_header, sizeof(struct iphdr));
    memcpy(packet + sizeof(struct iphdr), tcp_header, sizeof(struct tcphdr));

    if (sendto(sock_raw, packet, ip_header->tot_len, 0, (struct sockaddr *)&dest_info, sizeof(dest_info)) < 0) {
        std::cerr << "Send failed!\n";
        return;
    }

    std::cout << "Packet Sent!\n";

    close(sock_raw);
}

int main() {
    start_client();
    return 0;
}
