// sender.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>  // For IP header
#include <unistd.h>

// Checksum calculation function for the IP header
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) sum += *buf++;
    if (len == 1) sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sock;
    char packet[4096];
    struct sockaddr_in dest;
    struct iphdr *iph = (struct iphdr *)packet;

    // Create a raw socket with IPPROTO_RAW
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
    
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    	perror("setsockopt IP_HDRINCL failed");
    	return 1;
    }

    // Zero out the packet buffer
    memset(packet, 0, 4096);

    // Destination address
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("10.0.2.15");  // Receiver IP

    // Fill in the IP Header fields
    iph->ihl = 5;  // Internet Header Length
    iph->version = 4;  // IPv4
    iph->tos = 0;  // Type of Service
    iph->tot_len = sizeof(struct iphdr) + sizeof("Hello Receiver!");  // IP Header + Data
    iph->id = htonl(54321);  // Packet ID
    iph->frag_off = 0;  // Fragmentation Offset
    iph->ttl = 64;  // Time to Live
    iph->protocol = IPPROTO_RAW;  // Protocol (raw IP)
    iph->saddr = inet_addr("1.2.3.4");  // Sender IP
    iph->daddr = dest.sin_addr.s_addr;  // Receiver IP

    // Calculate the checksum for the IP header
    iph->check = checksum((unsigned short *)packet, iph->tot_len);

    // Data payload to send
    const char *data = "Hello Receiver!";
    memcpy(packet + sizeof(struct iphdr), data, strlen(data));

    // Send the packet
    if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
        return 1;
    }

    printf("Packet sent successfully!\n");
    close(sock);
    return 0;
}
