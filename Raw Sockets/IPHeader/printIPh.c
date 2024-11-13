// ip_header_sniffer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>   // Provides declarations for the IP header
#include <unistd.h>

// Function to print the IP header
void print_ip_header(unsigned char *buffer) {
    struct iphdr *iph = (struct iphdr *)buffer;
    struct sockaddr_in src, dest;

    // Get source and destination IP addresses
    src.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;

    printf("\n\nIP Header\n");
    printf("   |-IP Version        : %d\n", (unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
    printf("   |-Type of Service   : %d\n", (unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n", ntohs(iph->id));
    printf("   |-TTL               : %d\n", (unsigned int)iph->ttl);
    printf("   |-Protocol          : %d\n", (unsigned int)iph->protocol);
    printf("   |-Checksum          : %d\n", ntohs(iph->check));
    printf("   |-Source IP         : %s\n", inet_ntoa(src.sin_addr));
    printf("   |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
}

int main() {
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_len;
    unsigned char buffer[65536];

    // Create a raw socket to receive all packets at the IP level
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    printf("Starting packet capture...\n");

    // Loop to capture and process packets
    while (1) {
        addr_len = sizeof(addr);
        // Receive packets
        int packet_size = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
        if (packet_size < 0) {
            perror("Failed to receive packets");
            return 1;
        }

        // Print the IP header information
        print_ip_header(buffer);
    }

    close(sock);
    return 0;
}
