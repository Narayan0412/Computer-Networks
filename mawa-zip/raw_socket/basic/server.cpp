#include <iostream>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <unistd.h>

#define BUFFER_SIZE 65536

void print_ip_header(struct iphdr* ip_header) {
    struct in_addr src_addr, dest_addr;
    src_addr.s_addr = ip_header->saddr;
    dest_addr.s_addr = ip_header->daddr;

    std::cout << "IP Header Details:\n";
    std::cout << "Source IP: " << inet_ntoa(src_addr) << "\n";
    std::cout << "Destination IP: " << inet_ntoa(dest_addr) << "\n";
}

void print_ethernet_header(struct ethhdr* eth_header) {
    std::cout << "\nEthernet Header Details:\n";
    std::cout << "Source MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_source) << "\n";
    std::cout << "Destination MAC: " << ether_ntoa((struct ether_addr*)eth_header->h_dest) << "\n";
}

void start_server() {
    int sock_raw;
    struct sockaddr saddr;
    struct iphdr* ip_header;
    unsigned char* buffer = new unsigned char[BUFFER_SIZE];

    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0) {
        std::cerr << "Socket creation failed!\n";
        return;
    }

    std::cout << "Server is listening...\n";

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
        std::cout << "Packet received successfully!\n";
        std::cout << "---------------------------------------------\n";
    }

    close(sock_raw);
}

int main() {
    start_server();
    return 0;
}
