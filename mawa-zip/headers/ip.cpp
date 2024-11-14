#include <iostream>
#include <arpa/inet.h>
#include <netinet/ip.h>  // For struct ip
#include <netinet/in.h>  // For htons, ntohs
#include <cstring>

using namespace std;

// Structure for IP header
struct ip_header {
    u_int8_t ip_ver:4;        // IP version (4 for IPv4)
    u_int8_t ip_hdr_len:4;    // Header length in 32-bit words
    u_int8_t ip_tos;          // Type of service
    u_int16_t ip_total_len;   // Total length (header + data)
    u_int16_t ip_id;          // Identification
    u_int16_t ip_flags_frag;  // Flags and fragment offset
    u_int8_t ip_ttl;          // Time to live
    u_int8_t ip_protocol;     // Protocol (e.g., TCP = 6, UDP = 17)
    u_int16_t ip_checksum;    // Header checksum
    u_int32_t ip_src_addr;    // Source IP address
    u_int32_t ip_dest_addr;   // Destination IP address
};

// Function to print IP header
void print_ip_header(struct ip_header *ip) {
    cout << "IP Version: " << (int)ip->ip_ver << endl;
    cout << "Header Length: " << (int)ip->ip_hdr_len * 4 << " bytes" << endl;
    cout << "Type of Service: " << (int)ip->ip_tos << endl;
    cout << "Total Length: " << ntohs(ip->ip_total_len) << endl;
    cout << "Identification: " << ntohs(ip->ip_id) << endl;
    cout << "Flags and Fragment Offset: " << ntohs(ip->ip_flags_frag) << endl;
    cout << "Time to Live: " << (int)ip->ip_ttl << endl;
    cout << "Protocol: " << (int)ip->ip_protocol << endl;
    cout << "Checksum: " << ntohs(ip->ip_checksum) << endl;
    cout << "Source IP: " << inet_ntoa(*(struct in_addr *)&ip->ip_src_addr) << endl;
    cout << "Destination IP: " << inet_ntoa(*(struct in_addr *)&ip->ip_dest_addr) << endl;
}

int main() {
    // Create an example IP header
    struct ip_header ip;
    memset(&ip, 0, sizeof(ip));  // Initialize the header with zero

    // Fill in values for the IP header
    ip.ip_ver = 4;               // IPv4
    ip.ip_hdr_len = 5;           // 5 * 4 = 20 bytes (standard header length)
    ip.ip_tos = 0;               // Default type of service
    ip.ip_total_len = htons(40); // Total length (20 bytes header + 20 bytes data)
    ip.ip_id = htons(54321);     // Identification field (random value)
    ip.ip_flags_frag = 0;        // No fragmentation
    ip.ip_ttl = 64;              // Time to live (TTL)
    ip.ip_protocol = IPPROTO_TCP; // Protocol type (TCP = 6)
    ip.ip_checksum = 0;          // Checksum will be computed later
    ip.ip_src_addr = inet_addr("192.168.1.1"); // Source IP address
    ip.ip_dest_addr = inet_addr("192.168.1.2"); // Destination IP address

    // Print the IP header
    print_ip_header(&ip);

    return 0;
}
