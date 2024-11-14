#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <cstring>

#define ALLOWED_IP "192.168.1.100"  // Define allowed IP

// Calculate checksum for IP header and TCP header
unsigned short checksum(void *b, int len) {    
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }

    if (len == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to filter based on the source IP
bool is_allowed_ip(const std::string& ip_str) {
    return ip_str == ALLOWED_IP;
}

int main() {
    int sock_raw;
    struct sockaddr saddr;
    struct iphdr *ip_header = (struct iphdr*)malloc(sizeof(struct iphdr));
    struct ethhdr *eth_header = (struct ethhdr*)malloc(sizeof(struct ethhdr));
    char *buffer = (char*)malloc(65536); // Buffer to hold the incoming packet

    // Create raw socket
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Raw socket creation failed.\n";
        return 1;
    }

    while (true) {
        // Receive a packet
        int data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t*)sizeof(saddr));
        if (data_size < 0) {
            std::cerr << "Packet receiving failed.\n";
            return 1;
        }

        // Get the IP header part of the packet
        ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));  // Skip Ethernet header to get to IP header
        struct in_addr ip_addr;
        ip_addr.s_addr = ip_header->saddr;
        std::string source_ip = inet_ntoa(ip_addr);

        // Print incoming packet details
        std::cout << "Received packet from IP: " << source_ip << std::endl;

        // Check if the packet is from the allowed IP
        if (is_allowed_ip(source_ip)) {
            std::cout << "Packet allowed from IP: " << source_ip << std::endl;
            // Process the packet (e.g., send it to its destination)
        } else {
            std::cout << "Packet blocked from IP: " << source_ip << std::endl;
            // Drop the packet or take other actions if needed
        }
    }

    close(sock_raw);
    return 0;
}
