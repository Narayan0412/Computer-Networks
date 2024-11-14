#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

// ARP Header structure
struct arp_header {
    u_int16_t hw_type;       // Hardware type
    u_int16_t proto_type;    // Protocol type
    u_int8_t hw_len;         // Hardware address length
    u_int8_t proto_len;      // Protocol address length
    u_int16_t op_code;       // Operation code
    u_int8_t sender_mac[6];  // Sender MAC address
    u_int32_t sender_ip;     // Sender IP address
    u_int8_t target_mac[6];  // Target MAC address
    u_int32_t target_ip;     // Target IP address
};

// Function to print ARP header
void print_arp_header(struct arp_header *arp) {
    cout << "Hardware Type: " << ntohs(arp->hw_type) << endl;
    cout << "Protocol Type: " << ntohs(arp->proto_type) << endl;
    cout << "Hardware Address Length: " << (int)arp->hw_len << endl;
    cout << "Protocol Address Length: " << (int)arp->proto_len << endl;
    cout << "Operation Code: " << ntohs(arp->op_code) << endl;
    cout << "Sender MAC Address: ";
    for(int i = 0; i < 6; i++) cout << (int)arp->sender_mac[i] << (i < 5 ? ":" : "");
    cout << endl;
    cout << "Sender IP Address: " << inet_ntoa(*(struct in_addr *)&arp->sender_ip) << endl;
    cout << "Target MAC Address: ";
    for(int i = 0; i < 6; i++) cout << (int)arp->target_mac[i] << (i < 5 ? ":" : "");
    cout << endl;
    cout << "Target IP Address: " << inet_ntoa(*(struct in_addr *)&arp->target_ip) << endl;
}

int main() {
    // Create an example ARP header
    struct arp_header arp;
    memset(&arp, 0, sizeof(arp));

    // Fill in values for the ARP header
    arp.hw_type = htons(1);          // Ethernet
    arp.proto_type = htons(0x0800);  // IPv4
    arp.hw_len = 6;                  // MAC address length
    arp.proto_len = 4;               // IP address length
    arp.op_code = htons(1);          // ARP Request
    memcpy(arp.sender_mac, "\x00\x0c\x29\x2b\x3d\x42", 6);  // Sender MAC
    arp.sender_ip = inet_addr("192.168.1.1");  // Sender IP
    memcpy(arp.target_mac, "\x00\x00\x00\x00\x00\x00", 6); // Target MAC (unknown)
    arp.target_ip = inet_addr("192.168.1.2");  // Target IP

    // Print the ARP header
    print_arp_header(&arp);

    return 0;
}
