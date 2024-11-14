#include <iostream>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 65536

// Function to print Ethernet header
void print_ethernet_header(struct ethhdr* eth_header) {
    std::cout << "\nEthernet Header:\n";
    std::cout << "Source MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_source) << "\n";
    std::cout << "Destination MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_dest) << "\n";
}

// Function to print IP header
void print_ip_header(struct iphdr* ip_header) {
    struct in_addr src_addr, dest_addr;
    src_addr.s_addr = ip_header->saddr;
    dest_addr.s_addr = ip_header->daddr;

    std::cout << "\nIP Header:\n";
    std::cout << "IP Version: " << (unsigned int)ip_header->version << "\n";
    std::cout << "Source IP: " << inet_ntoa(src_addr) << "\n";
    std::cout << "Destination IP: " << inet_ntoa(dest_addr) << "\n";
    std::cout << "TTL: " << (unsigned int)ip_header->ttl << "\n";
    std::cout << "Protocol: " << (unsigned int)ip_header->protocol << "\n";
}

// Function to print TCP header
void print_tcp_header(struct tcphdr* tcp_header) {
    std::cout << "\nTCP Header:\n";
    std::cout << "Source Port: " << ntohs(tcp_header->source) << "\n";
    std::cout << "Destination Port: " << ntohs(tcp_header->dest) << "\n";
    std::cout << "Sequence Number: " << ntohl(tcp_header->seq) << "\n";
    std::cout << "Acknowledgment Number: " << ntohl(tcp_header->ack_seq) << "\n";
    std::cout << "Data Offset: " << (unsigned int)tcp_header->doff << "\n";
    std::cout << "Flags: ";
    if (tcp_header->syn) std::cout << "SYN ";
    if (tcp_header->ack) std::cout << "ACK ";
    if (tcp_header->fin) std::cout << "FIN ";
    if (tcp_header->rst) std::cout << "RST ";
    std::cout << "\n";
}

// Function to print the payload (data part of the packet)
void print_payload(unsigned char* buffer, int ip_header_len, int data_size) {
    unsigned char* payload = buffer + ip_header_len;
    int payload_size = data_size - ip_header_len;

    if (payload_size > 0) {
        std::cout << "\nPayload:\n";
        for (int i = 0; i < payload_size; i++) {
            std::cout << std::hex << (int)payload[i] << " ";
            if ((i + 1) % 16 == 0) {
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }
}

// Function to start the sniffer
void start_sniffer() {
    int sock_raw;
    struct sockaddr saddr;
    unsigned char* buffer = new unsigned char[BUFFER_SIZE];

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // Capture all Ethernet frames
    if (sock_raw < 0) {
        std::cerr << "Socket creation failed!\n";
        return;
    }

    std::cout << "Sniffer is listening...\n";

    while (true) {
        ssize_t data_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, &saddr, nullptr);
        if (data_size < 0) {
            std::cerr << "Packet capture failed\n";
            return;
        }

        struct ethhdr* eth_header = (struct ethhdr*)buffer;
        if (eth_header->h_proto != htons(ETH_P_IP)) {
            continue; // Only process IP packets
        }

        struct iphdr* ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));

        print_ethernet_header(eth_header);
        print_ip_header(ip_header);

        // Calculate the IP header length
        int ip_header_len = ip_header->ihl * 4;

        // Check for TCP
        if (ip_header->protocol == IPPROTO_TCP) {
            struct tcphdr* tcp_header = (struct tcphdr*)(buffer + ip_header_len);
            print_tcp_header(tcp_header);
            print_payload(buffer, ip_header_len + (tcp_header->doff * 4), data_size);
        } else {
            std::cout << "\nNon-TCP Packet. Skipping Payload.\n";
        }

        std::cout << "---------------------------------------------\n";
    }

    close(sock_raw);
}

int main() {
    start_sniffer();
    return 0;
}
