#include <iostream>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

// Ethernet Header structure
struct eth_header {
    u_int8_t dest_mac[6];    // Destination MAC address
    u_int8_t src_mac[6];     // Source MAC address
    u_int16_t eth_type;      // Ethernet type
};

// Function to print Ethernet header
void print_eth_header(struct eth_header *eth) {
    cout << "Destination MAC Address: ";
    for(int i = 0; i < 6; i++) cout << (int)eth->dest_mac[i] << (i < 5 ? ":" : "");
    cout << endl;
    cout << "Source MAC Address: ";
    for(int i = 0; i < 6; i++) cout << (int)eth->src_mac[i] << (i < 5 ? ":" : "");
    cout << endl;
    cout << "Ethernet Type: " << ntohs(eth->eth_type) << endl;
}

int main() {
    // Create an example Ethernet header
    struct eth_header eth;
    memset(&eth, 0, sizeof(eth));

    // Fill in values for the Ethernet header
    memcpy(eth.dest_mac, "\x00\x0c\x29\x2b\x3d\x42", 6);  // Destination MAC
    memcpy(eth.src_mac, "\x00\x0c\x29\x2b\x3d\x43", 6);   // Source MAC
    eth.eth_type = htons(0x0800);                          // Ethernet Type (IPv4)

    // Print the Ethernet header
    print_eth_header(&eth);

    return 0;
}
