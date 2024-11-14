#include <iostream>
#include <cstring>
#include <netinet/ip.h>      // for struct ip
#include <netinet/tcp.h>     // for struct tcphdr
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/ethernet.h>    // for struct ether_header

// Function to calculate the checksum for the IP header
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to print Ethernet header
void print_ethernet_header(const struct ether_header *eth) {
    std::cout << "Ethernet Header\n";
    std::cout << "  Destination MAC: " 
              << std::hex << (int)eth->ether_dhost[0] << ":"
              << (int)eth->ether_dhost[1] << ":"
              << (int)eth->ether_dhost[2] << ":"
              << (int)eth->ether_dhost[3] << ":"
              << (int)eth->ether_dhost[4] << ":"
              << (int)eth->ether_dhost[5] << std::dec << "\n";
    std::cout << "  Source MAC: " 
              << std::hex << (int)eth->ether_shost[0] << ":"
              << (int)eth->ether_shost[1] << ":"
              << (int)eth->ether_shost[2] << ":"
              << (int)eth->ether_shost[3] << ":"
              << (int)eth->ether_shost[4] << ":"
              << (int)eth->ether_shost[5] << std::dec << "\n";
    std::cout << "  Ethernet Type: " << std::hex << ntohs(eth->ether_type) << std::dec << "\n";
}

// Function to print IP header
void print_ip_header(const struct ip *ip_header) {
    std::cout << "\nIP Header\n";
    std::cout << "  Source IP: " << inet_ntoa(ip_header->ip_src) << "\n";
    std::cout << "  Destination IP: " << inet_ntoa(ip_header->ip_dst) << "\n";
    std::cout << "  Protocol: " << (int)ip_header->ip_p << "\n";
}

// Function to print TCP header
void print_tcp_header(const struct tcphdr *tcp_header) {
    std::cout << "\nTCP Header\n";
    std::cout << "  Source Port: " << ntohs(tcp_header->th_sport) << "\n";
    std::cout << "  Destination Port: " << ntohs(tcp_header->th_dport) << "\n";
    std::cout << "  Sequence Number: " << ntohl(tcp_header->th_seq) << "\n";
    std::cout << "  Acknowledgment Number: " << ntohl(tcp_header->th_ack) << "\n";
    std::cout << "  Data Offset: " << (int)(tcp_header->th_off) << "\n";
    std::cout << "  Flags: " << std::bitset<9>(tcp_header->th_flags) << "\n";
}

// Function to print the payload (data part of the packet)
void print_payload(const char *buffer, int payload_len) {
    std::cout << "\nPayload (Data)\n";
    for (int i = 0; i < payload_len; i++) {
        std::cout << buffer[i];
    }
    std::cout << "\n";
}

// Function to send the sniffed packet to the destination
void forward_packet(int sockfd, const char *packet, ssize_t packet_size, struct ip *ip_header) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = ip_header->ip_p;  // port number
    dest_addr.sin_addr = ip_header->ip_dst;

    // Send the packet to the destination
    ssize_t sent_size = sendto(sockfd, packet, packet_size, 0, 
                                (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent_size < 0) {
        std::cerr << "Error forwarding the packet." << std::endl;
    }
}

int main() {
    int sockfd;
    char buffer[65536];  // buffer to store packet

    // Create a raw socket to sniff packets
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // Create a socket to forward the sniffed packet to the destination
    int forward_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (forward_sockfd < 0) {
        std::cerr << "Socket creation for forwarding failed!" << std::endl;
        return 1;
    }

    while (true) {
        // Receive the packet
        ssize_t packet_size = recv(sockfd, buffer, sizeof(buffer), 0);
        if (packet_size < 0) {
            std::cerr << "Packet receive failed!" << std::endl;
            continue;
        }

        struct ether_header *eth_header = (struct ether_header *) buffer;
        struct ip *ip_header = (struct ip *)(buffer + sizeof(struct ether_header));
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct ether_header) + sizeof(struct ip));

        // Print the Ethernet header
        print_ethernet_header(eth_header);

        // Print the IP header
        print_ip_header(ip_header);

        // Print the TCP header if it is a TCP packet
        if (ip_header->ip_p == IPPROTO_TCP) {
            print_tcp_header(tcp_header);
        }

        // Calculate the payload size and print it
        int ip_header_len = ip_header->ip_hl * 4;  // IP header length
        int tcp_header_len = tcp_header->th_off * 4;  // TCP header length
        int payload_len = packet_size - (sizeof(struct ether_header) + ip_header_len + tcp_header_len);
        print_payload(buffer + sizeof(struct ether_header) + ip_header_len + tcp_header_len, payload_len);

        // Forward the sniffed packet to the actual destination
        forward_packet(forward_sockfd, buffer, packet_size, ip_header);
    }

    // Close the sockets
    close(sockfd);
    close(forward_sockfd);
    return 0;
}
