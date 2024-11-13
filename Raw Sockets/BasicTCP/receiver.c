#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>  // Provides declarations for TCP header
#include <netinet/ip.h>   // Provides declarations for IP header
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[4096];
    socklen_t addr_len = sizeof(client_addr);

    // Create a raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces
    server_addr.sin_port = htons(8080);  // Port to listen on

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("Waiting for incoming packets...\n");

    // Loop to receive packets
    while (1) {
        // Receive packets
        int packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (packet_size < 0) {
            perror("Failed to receive packets");
            return 1;
        }

        // Extract IP header and TCP header
        struct iphdr *iph = (struct iphdr *)buffer;
        struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct iphdr) * 2);

        // Calculate the start of the data (payload)
        char *data = buffer + sizeof(struct iphdr) + sizeof(struct tcphdr) * 2;
        printf("Received packet:\n");
        printf("   |-Source IP         : %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
        printf("   |-Destination IP    : %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));
        printf("   |-Source Port       : %d\n", ntohs(tcph->source));
        printf("   |-Destination Port  : %d\n", ntohs(tcph->dest));
        printf("   |-Payload           : %s\n", data); // Print the received payload
    }

    close(sockfd);
    return 0;
}
