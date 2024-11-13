#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 65536

int main() {
    int sock_raw;
    struct sockaddr_ll saddr;
    int saddr_len = sizeof(saddr);
    unsigned char buffer[BUFFER_SIZE];

    // Create a raw socket to listen for all Ethernet packets
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket Error");
        return 1;
    }

    printf("Listening for incoming packets...\n");

    // Receive packets in a loop
    while (1) {
        int data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&saddr, (socklen_t *)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom Error");
            return 1;
        }

        // Print the Ethernet header
        struct ethhdr *eth = (struct ethhdr *)buffer;
        printf("\nEthernet Packet received!\n");
        printf("   |-Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], 
                eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
        printf("   |-Source MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n", 
                eth->h_source[0], eth->h_source[1], eth->h_source[2], 
                eth->h_source[3], eth->h_source[4], eth->h_source[5]);
        printf("   |-EtherType: 0x%04x\n", ntohs(eth->h_proto));

        // Print payload or additional processing here
    }

    close(sock_raw);
    return 0;
}
