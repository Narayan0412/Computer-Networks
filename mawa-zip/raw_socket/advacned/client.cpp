#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <cstring>
#include <unistd.h>

// Pseudo header needed for TCP checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// Calculate checksum
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

int main() {
    int sock_raw;
    struct sockaddr_in dest;
    struct iphdr *ip_header = (struct iphdr*)malloc(sizeof(struct iphdr));
    struct tcphdr *tcp_header = (struct tcphdr*)malloc(sizeof(struct tcphdr));
    struct ethhdr *eth_header = (struct ethhdr*)malloc(sizeof(struct ethhdr));
    char *packet = (char*)malloc(4096); // Packet buffer

    // Creating raw socket
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    dest.sin_family = AF_INET;
    dest.sin_port = htons(80);  // Destination port (HTTP)
    dest.sin_addr.s_addr = inet_addr("192.168.1.100");  // Destination IP

    // Ethernet Header
    memset(eth_header, 0, sizeof(struct ethhdr));
    eth_header->h_proto = htons(ETH_P_IP);  // IP protocol

    // IP Header
    memset(ip_header, 0, sizeof(struct iphdr));
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr); // Total packet size
    ip_header->id = htonl(54321);  // Random ID
    ip_header->frag_off = 0;
    ip_header->ttl = 255;
    ip_header->protocol = IPPROTO_TCP;
    ip_header->check = 0;  // Leave checksum 0 now, filled later by pseudo header
    ip_header->saddr = inet_addr("192.168.1.1");  // Source IP
    ip_header->daddr = dest.sin_addr.s_addr;

    // TCP Header
    memset(tcp_header, 0, sizeof(struct tcphdr));
    tcp_header->source = htons(12345);  // Source port
    tcp_header->dest = htons(80);  // Destination port (HTTP)
    tcp_header->seq = 0;
    tcp_header->ack_seq = 0;
    tcp_header->doff = 5;  // TCP header size
    tcp_header->fin = 0;
    tcp_header->syn = 1;  // SYN flag to initiate the connection
    tcp_header->rst = 0;
    tcp_header->psh = 0;
    tcp_header->ack = 0;
    tcp_header->urg = 0;
    tcp_header->window = htons(5840); /* maximum allowed window size */
    tcp_header->check = 0;  // Leave checksum 0 now, filled later by pseudo header
    tcp_header->urg_ptr = 0;

    // Pseudo header needed for checksum calculation
    struct pseudo_header psh;
    psh.source_address = inet_addr("192.168.1.1");  // Source IP
    psh.dest_address = dest.sin_addr.s_addr;  // Destination IP
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
    char *pseudogram = (char*)malloc(psize);

    // Copy pseudo header and TCP header to create pseudogram
    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcp_header, sizeof(struct tcphdr));

    // Calculate checksum for TCP
    tcp_header->check = checksum((unsigned short*)pseudogram, psize);

    // IP Header checksum
    ip_header->check = checksum((unsigned short*)ip_header, sizeof(struct iphdr));

    // Copy headers to packet buffer
    memcpy(packet, eth_header, sizeof(struct ethhdr)); // Ethernet header
    memcpy(packet + sizeof(struct ethhdr), ip_header, sizeof(struct iphdr)); // IP header
    memcpy(packet + sizeof(struct ethhdr) + sizeof(struct iphdr), tcp_header, sizeof(struct tcphdr)); // TCP header

    // Send the packet
    if (sendto(sock_raw, packet, ip_header->tot_len + sizeof(struct ethhdr), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        std::cerr << "Packet send failed\n";
        return 1;
    }

    std::cout << "Packet Sent\n";

    close(sock_raw);
    return 0;
}
