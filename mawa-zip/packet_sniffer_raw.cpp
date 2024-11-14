#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#define BUFFER_SIZE 65536

// Function to print the IP header details
void print_ip_header(struct iphdr* ip_header) {
    struct in_addr src_addr, dest_addr;
    src_addr.s_addr = ip_header->saddr;
    dest_addr.s_addr = ip_header->daddr;

    std::cout << "IP Header Details:\n";
    std::cout << "Version: " << (unsigned int)ip_header->version << "\n";
    std::cout << "IP Header Length: " << (unsigned int)ip_header->ihl << " words\n";
    std::cout << "Source IP: " << inet_ntoa(src_addr) << "\n";
    std::cout << "Destination IP: " << inet_ntoa(dest_addr) << "\n";
    std::cout << "Protocol: " << (unsigned int)ip_header->protocol << "\n";
}

// Function to print the Ethernet header details
void print_ethernet_header(struct ethhdr* eth_header) {
    std::cout << "\nEthernet Header Details:\n";
    std::cout << "Destination MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_dest) << "\n";
    std::cout << "Source MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_source) << "\n";
}

// Function to print the payload (data) in a readable format
void print_payload(const unsigned char* buffer, int size) {
    std::cout << "Payload Data (hex):\n";
    for (int i = 0; i < size; i++) {
        if (i != 0 && i % 16 == 0) {
            std::cout << std::endl;
        }
        std::cout << std::hex << (int)buffer[i] << " ";
    }
    std::cout << "\n";
}

// Function to start sniffing
void start_sniffing() {
    int sock_raw;
    struct sockaddr saddr;
    struct iphdr* ip_header;
    struct tcphdr* tcp_header;
    struct udphdr* udp_header;
    unsigned char* buffer = new unsigned char[BUFFER_SIZE];

    // Create a raw socket to capture packets
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Socket creation failed!\n";
        return;
    }

    std::cout << "Started sniffing...\n";

    // Continuously capture packets
    while (true) {
        ssize_t data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, nullptr);
        if (data_size < 0) {
            std::cerr << "Packet capture failed\n";
            return;
        }

        struct ethhdr* eth_header = (struct ethhdr*)buffer;
        ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));

        print_ethernet_header(eth_header);
        print_ip_header(ip_header);

        // Get the IP header length (ihl) to calculate the total length of the IP header
        int ip_header_len = ip_header->ihl * 4;

        // Calculate where the data (payload) starts
        unsigned char* payload = buffer + sizeof(struct ethhdr) + ip_header_len;

        // Check if the protocol is TCP or UDP and print payload accordingly
        if (ip_header->protocol == IPPROTO_TCP) {
            tcp_header = (struct tcphdr*)(buffer + ip_header_len);
            int tcp_header_len = tcp_header->doff * 4;  // TCP header length
            unsigned char* tcp_payload = payload + tcp_header_len;
            int tcp_payload_len = data_size - (sizeof(struct ethhdr) + ip_header_len + tcp_header_len);
            print_payload(tcp_payload, tcp_payload_len);
        } 
        else if (ip_header->protocol == IPPROTO_UDP) {
            udp_header = (struct udphdr*)(buffer + ip_header_len);
            unsigned char* udp_payload = payload + sizeof(struct udphdr);
            int udp_payload_len = data_size - (sizeof(struct ethhdr) + ip_header_len + sizeof(struct udphdr));
            print_payload(udp_payload, udp_payload_len);
        }
        else {
            std::cout << "Other Protocol (non-TCP/UDP) detected, skipping payload...\n";
        }

        std::cout << "---------------------------------------------\n";
    }

    close(sock_raw);
}

int main() {
    start_sniffing();
    return 0;
}
