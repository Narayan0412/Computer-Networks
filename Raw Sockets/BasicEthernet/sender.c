#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>   // For Ethernet header
#include <netpacket/packet.h> // For sockaddr_ll
#include <net/if.h>          // For network interface
#include <sys/ioctl.h>       // For ioctl

#define DEST_MAC0    0x08
#define DEST_MAC1    0x00
#define DEST_MAC2    0x27
#define DEST_MAC3    0xad
#define DEST_MAC4    0x25
#define DEST_MAC5    0x87

#define ETHER_TYPE   0x0800 // IPv4

#define INTERFACE    "eth0" // Change this to the appropriate interface name

int main() {
    int sock_raw;
    unsigned char buffer[ETH_FRAME_LEN];
    struct ifreq ifr;
    struct sockaddr_ll socket_address;
    int ifindex, send_result;

    // Create a raw socket
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Get the interface index for the specified interface
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, INTERFACE, IFNAMSIZ - 1);
    if (ioctl(sock_raw, SIOCGIFINDEX, &ifr) == -1) {
        perror("Error getting interface index");
        close(sock_raw);
        return 1;
    }
    ifindex = ifr.ifr_ifindex;

    // Prepare the Ethernet frame
    // Destination MAC: 08:00:27:cb:7e:f5
    buffer[0] = DEST_MAC0;
    buffer[1] = DEST_MAC1;
    buffer[2] = DEST_MAC2;
    buffer[3] = DEST_MAC3;
    buffer[4] = DEST_MAC4;
    buffer[5] = DEST_MAC5;

    // Source MAC: Get it from the interface
    if (ioctl(sock_raw, SIOCGIFHWADDR, &ifr) == -1) {
        perror("Error getting source MAC address");
        close(sock_raw);
        return 1;
    }
    memcpy(buffer + 6, ifr.ifr_hwaddr.sa_data, 6);

    // Ethernet type (IPv4 in this case)
    buffer[12] = (ETHER_TYPE >> 8) & 0xFF;
    buffer[13] = ETHER_TYPE & 0xFF;

    // Add the payload (data to send in the packet)
    const char *payload = "Hello, Ethernet!";
    int payload_size = strlen(payload);
    memcpy(buffer + 14, payload, payload_size);

    // Define the total frame size (14 bytes header + payload)
    int frame_size = 14 + payload_size;

    // Set up the destination address struct
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sll_ifindex = ifindex;  // Interface index
    socket_address.sll_halen = ETH_ALEN;   // Address length (6 bytes)
    memcpy(socket_address.sll_addr, buffer, 6);  // Destination MAC address

    // Send the packet
    send_result = sendto(sock_raw, buffer, frame_size, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if (send_result < 0) {
        perror("Send failed");
        close(sock_raw);
        return 1;
    }

    printf("Packet sent to 08:00:27:cb:7e:f5\n");

    // Close the raw socket
    close(sock_raw);
    return 0;
}
