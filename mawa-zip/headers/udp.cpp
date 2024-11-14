#include <iostream>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <cstring>

using namespace std;

// Structure for UDP header
struct udp_header {
    u_int16_t source_port;    // Source Port
    u_int16_t dest_port;      // Destination Port
    u_int16_t length;         // Length of UDP header and data
    u_int16_t checksum;       // Checksum
};

// Function to print UDP header
void print_udp_header(struct udp_header *udp) {
    cout << "Source Port: " << ntohs(udp->source_port) << endl;
    cout << "Destination Port: " << ntohs(udp->dest_port) << endl;
    cout << "Length: " << ntohs(udp->length) << endl;
    cout << "Checksum: " << ntohs(udp->checksum) << endl;
}

int main() {
    // Create an example UDP header
    struct udp_header udp;
    memset(&udp, 0, sizeof(udp));

    // Fill in values for the UDP header
    udp.source_port = htons(12345);   // Source port
    udp.dest_port = htons(80);        // Destination port
    udp.length = htons(8 + 20);       // Length of UDP header + data (example: 28 bytes)
    udp.checksum = 0;                 // Checksum (placeholder)

    // Print the UDP header
    print_udp_header(&udp);

    return 0;
}
