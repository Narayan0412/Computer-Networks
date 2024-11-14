#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <map>
#include <ctime>

#define ALLOWED_IP "192.168.1.100"   // Define allowed IP
#define LOG_FILE "firewall.log"       // Log file for allowed/blocked packets

// Logging function
void log_packet(const std::string &message) {
    std::ofstream log_file;
    log_file.open(LOG_FILE, std::ios_base::app);
    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';  // Remove newline
    log_file << "[" << dt << "] " << message << std::endl;
    log_file.close();
}

// Check if a given IP is in the allowed range (CIDR)
bool is_allowed_ip(const std::string& ip_str) {
    return ip_str == ALLOWED_IP;
}

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

// Function to parse and filter IP packets
bool process_packet(char *buffer) {
    struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));  // Skip Ethernet header
    struct in_addr ip_addr;
    ip_addr.s_addr = ip_header->saddr;
    std::string source_ip = inet_ntoa(ip_addr);

    // Filter based on source IP address
    if (!is_allowed_ip(source_ip)) {
        log_packet("Blocked packet from IP: " + source_ip);
        return false;  // Block packet
    }

    // If the packet is allowed, print the details
    std::cout << "Packet allowed from IP: " << source_ip << std::endl;

    // Further filtering based on protocol (TCP/UDP/ICMP)
    if (ip_header->protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
        std::cout << "TCP Packet: Source Port: " << ntohs(tcp_header->source)
                  << ", Destination Port: " << ntohs(tcp_header->dest) << std::endl;
    } else if (ip_header->protocol == IPPROTO_UDP) {
        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
        std::cout << "UDP Packet: Source Port: " << ntohs(udp_header->source)
                  << ", Destination Port: " << ntohs(udp_header->dest) << std::endl;
    } else if (ip_header->protocol == IPPROTO_ICMP) {
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
        std::cout << "ICMP Packet: Type: " << (int)icmp_header->type << ", Code: " << (int)icmp_header->code << std::endl;
    } else {
        std::cout << "Unsupported Protocol" << std::endl;
    }

    log_packet("Allowed packet from IP: " + source_ip);
    return true;  // Allow packet
}

int main() {
    int sock_raw;
    struct sockaddr saddr;
    char *buffer = (char*)malloc(65536);  // Buffer to hold incoming packet

    // Create raw socket
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Raw socket creation failed.\n";
        return 1;
    }

    // Map to store IP address rate limits (e.g., number of packets per minute)
    std::map<std::string, int> ip_rate_limit;
    time_t start_time = time(NULL);

    while (true) {
        // Receive a packet
        int data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t*)sizeof(saddr));
        if (data_size < 0) {
            std::cerr << "Packet receiving failed.\n";
            continue;
        }

        // Check rate limiting (1 packet per second for now)
        time_t current_time = time(NULL);
        if (current_time - start_time >= 60) {
            ip_rate_limit.clear();  // Reset every minute
            start_time = current_time;
        }

        struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));  // Skip Ethernet header
        struct in_addr ip_addr;
        ip_addr.s_addr = ip_header->saddr;
        std::string source_ip = inet_ntoa(ip_addr);

        // Rate limiting check
        if (ip_rate_limit[source_ip] >= 5) {  // Limit to 5 packets per minute from a single IP
            log_packet("Rate limit exceeded for IP: " + source_ip);
            std::cout << "Rate limit exceeded for IP: " << source_ip << std::endl;
            continue;
        }

        // Process the packet
        if (process_packet(buffer)) {
            // Increment rate limit counter for the source IP
            ip_rate_limit[source_ip]++;
        }
    }

    close(sock_raw);
    return 0;
}
