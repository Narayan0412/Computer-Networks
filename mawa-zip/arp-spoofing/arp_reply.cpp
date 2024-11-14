#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define SRC_IP "192.168.1.1"   // Source IP address
#define DST_IP "192.168.1.100" // Destination IP address for ARP reply

// Define ARP Reply packet structure
struct arp_packet {
    struct ether_header eth_hdr;
    struct ether_arp arp_hdr;
};

int main() {
    int sock;
    struct sockaddr_ll sa;
    struct arp_packet packet;
    struct ifreq ifr;

    // Create a raw socket for Ethernet frame
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    // Get the network interface details (e.g., eth0)
    strcpy(ifr.ifr_name, "eth0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    // Set up sockaddr_ll to specify interface for sending the packet
    memset(&sa, 0, sizeof(struct sockaddr_ll));
    sa.sll_ifindex = ifr.ifr_ifindex;  // Network interface index

    // Ethernet header
    memset(&packet.eth_hdr, 0, sizeof(struct ether_header));
    packet.eth_hdr.ether_dhost[0] = 0x00;  // Destination MAC address (set by ARP Requester)
    packet.eth_hdr.ether_dhost[1] = 0x11;
    packet.eth_hdr.ether_dhost[2] = 0x22;
    packet.eth_hdr.ether_dhost[3] = 0x33;
    packet.eth_hdr.ether_dhost[4] = 0x44;
    packet.eth_hdr.ether_dhost[5] = 0x55;
    packet.eth_hdr.ether_shost[0] = 0x00;  // Source MAC address (replying MAC)
    packet.eth_hdr.ether_shost[1] = 0x66;
    packet.eth_hdr.ether_shost[2] = 0x77;
    packet.eth_hdr.ether_shost[3] = 0x88;
    packet.eth_hdr.ether_shost[4] = 0x99;
    packet.eth_hdr.ether_shost[5] = 0xAA;
    packet.eth_hdr.ether_type = htons(ETH_P_ARP);  // ARP Protocol type

    // ARP header
    memset(&packet.arp_hdr, 0, sizeof(struct ether_arp));
    packet.arp_hdr.arp_hrd = htons(ARPHRD_ETHER);  // Hardware type (Ethernet)
    packet.arp_hdr.arp_pro = htons(ETH_P_IP);     // Protocol type (IPv4)
    packet.arp_hdr.arp_hln = 6;                   // MAC address length
    packet.arp_hdr.arp_pln = 4;                   // IP address length
    packet.arp_hdr.arp_op = htons(ARPOP_REPLY);   // ARP Reply operation

    // Fill in sender and target fields in ARP reply
    packet.arp_hdr.arp_sha[0] = 0x00;  // Sender MAC address (to be changed)
    packet.arp_hdr.arp_sha[1] = 0x66;
    packet.arp_hdr.arp_sha[2] = 0x77;
    packet.arp_hdr.arp_sha[3] = 0x88;
    packet.arp_hdr.arp_sha[4] = 0x99;
    packet.arp_hdr.arp_sha[5] = 0xAA;  // Your own MAC address

    inet_pton(AF_INET, DST_IP, &packet.arp_hdr.arp_spa);  // Sender IP (Source IP)

    packet.arp_hdr.arp_tha[0] = 0x00;  // Target MAC (Sender MAC from ARP Request)
    packet.arp_hdr.arp_tha[1] = 0x11;
    packet.arp_hdr.arp_tha[2] = 0x22;
    packet.arp_hdr.arp_tha[3] = 0x33;
    packet.arp_hdr.arp_tha[4] = 0x44;
    packet.arp_hdr.arp_tha[5] = 0x55;

    inet_pton(AF_INET, DST_IP, &packet.arp_hdr.arp_tpa);  // Target IP (to respond to ARP)

    // Send the ARP reply
    if (sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll)) < 0) {
        std::cerr << "Send failed!" << std::endl;
        return -1;
    }

    std::cout << "ARP Reply sent!" << std::endl;

    close(sock);
    return 0;
}
