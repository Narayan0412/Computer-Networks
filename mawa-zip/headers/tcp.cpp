#include <iostream>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <cstring>

using namespace std;

// Structure for TCP header
struct tcp_header {
    u_int16_t source_port;    // Source Port
    u_int16_t dest_port;      // Destination Port
    u_int32_t seq_num;        // Sequence Number
    u_int32_t ack_num;        // Acknowledgment Number
    u_int8_t data_offset:4;   // Data offset (4 bits)
    u_int8_t reserved:3;      // Reserved bits
    u_int8_t flags:9;         // Flags (9 bits)
    u_int16_t window_size;    // Window Size
    u_int16_t checksum;       // Checksum
    u_int16_t urgent_ptr;     // Urgent Pointer
};

// Function to print TCP header
void print_tcp_header(struct tcp_header *tcp) {
    cout << "Source Port: " << ntohs(tcp->source_port) << endl;
    cout << "Destination Port: " << ntohs(tcp->dest_port) << endl;
    cout << "Sequence Number: " << ntohl(tcp->seq_num) << endl;
    cout << "Acknowledgment Number: " << ntohl(tcp->ack_num) << endl;
    cout << "Data Offset: " << (int)tcp->data_offset * 4 << " bytes" << endl;
    cout << "Flags: " << (int)tcp->flags << endl;
    cout << "Window Size: " << ntohs(tcp->window_size) << endl;
    cout << "Checksum: " << ntohs(tcp->checksum) << endl;
    cout << "Urgent Pointer: " << ntohs(tcp->urgent_ptr) << endl;
}

int main() {
    // Create an example TCP header
    struct tcp_header tcp;
    memset(&tcp, 0, sizeof(tcp));

    // Fill in values for the TCP header
    tcp.source_port = htons(12345);   // Source port
    tcp.dest_port = htons(80);        // Destination port
    tcp.seq_num = htonl(1000);        // Sequence number
    tcp.ack_num = htonl(2000);        // Acknowledgment number
    tcp.data_offset = 5;              // Data offset (5 * 4 = 20 bytes)
    tcp.flags = 2;                    // SYN flag
    tcp.window_size = htons(65535);   // Window size
    tcp.checksum = 0;                 // Checksum (placeholder)
    tcp.urgent_ptr = 0;               // Urgent pointer

    // Print the TCP header
    print_tcp_header(&tcp);

    return 0;
}
