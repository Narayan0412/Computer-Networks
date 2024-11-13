#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>  // Provides declarations for TCP header
#include <netinet/ip.h>   // Provides declarations for IP header
#include <unistd.h>

// Pseudo header needed for TCP checksum calculation
struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sock;
    struct sockaddr_in dest;

    // Create a raw socket
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Buffer for the packet
    char packet[4096];  // Large buffer to hold IP and TCP headers
    memset(packet, 0, 4096);  // Zero out the buffer

    // IP header
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct iphdr));
    struct pseudo_header psh;

    // Fill in the IP Header
    iph->ihl = 5;  // Header length
    iph->version = 4;  // IPv4
    iph->tos = 0;  // Type of service
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen("Hello, TCP!") + 1;  // IP header + TCP header + payload
    iph->id = htonl(54321);  // Identification
    iph->frag_off = 0;  // No fragmentation
    iph->ttl = 255;  // Time to live
    iph->protocol = IPPROTO_TCP;  // TCP
    iph->check = 0;  // Set to 0 before calculating checksum
    iph->saddr = inet_addr("10.0.2.15");  // Source IP (change this if necessary)
    iph->daddr = inet_addr("127.0.0.1");  // Destination IP (change this if necessary)

    // IP checksum
    iph->check = checksum((unsigned short *)packet, iph->tot_len);

    // Fill in the TCP Header
    tcph->source = htons(12345);  // Source port
    tcph->dest = htons(8080);  // Destination port 
    tcph->seq = 0;  // Sequence number
    tcph->ack_seq = 0;  // Acknowledgment number (not used)
    tcph->doff = 5;  // Data offset (TCP header size)
    tcph->fin = 0;  // Finish flag
    tcph->syn = 1;  // Synchronize flag (to initiate connection)
    tcph->rst = 0;  // Reset flag
    tcph->psh = 0;  // Push flag
    tcph->ack = 0;  // Acknowledgment flag
    tcph->urg = 0;  // Urgent flag
    tcph->window = htons(5840);  // Window size
    tcph->check = 0;  // Checksum (initially 0)
    tcph->urg_ptr = 0;  // Urgent pointer

    // Payload
    const char *payload = "Hello, TCP!";
    memcpy(packet + sizeof(struct iphdr) + sizeof(struct tcphdr), payload, strlen(payload) + 1); // +1 for null terminator

    // Now we need to calculate the TCP checksum which requires a pseudo-header
    psh.source_address = inet_addr("10.0.2.15");
    psh.dest_address = inet_addr("127.0.0.1");
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(payload)); // Include payload length

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(payload);
    char *pseudogram = malloc(psize);
    if (pseudogram == NULL) {
        perror("Memory allocation failed");
        close(sock);
        return 1;
    }

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr));
    memcpy(pseudogram + sizeof(struct pseudo_header) + sizeof(struct tcphdr), payload, strlen(payload) + 1); // Include payload

    tcph->check = checksum((unsigned short *)pseudogram, psize);

    // Destination address setup
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8080);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send the packet
    if (sendto(sock, packet, iph->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
    } else {
        printf("Packet Sent: %s\n", payload);
    }

    free(pseudogram);
    close(sock);
    return 0;
}
