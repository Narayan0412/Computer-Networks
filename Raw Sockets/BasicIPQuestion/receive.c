// receiver.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>  // For IP header

int main() {
    int sock;
    char buffer[4096];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    // Create a raw socket to receive IP packets
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    printf("Waiting for incoming packets...\n");

    // Receive a raw packet
    if (recvfrom(sock, buffer, 4096, 0, (struct sockaddr *)&sender, &sender_len) < 0) {
        perror("Receive failed");
        return 1;
    }

    // Print the received packet data (payload starts after the IP header)
    struct iphdr *iph = (struct iphdr *)buffer;
    char *data = buffer + sizeof(struct iphdr);

    printf("Received packet from: %s\n", inet_ntoa(sender.sin_addr));
    printf("Data: %s\n", data);

    close(sock);
    return 0;
}
