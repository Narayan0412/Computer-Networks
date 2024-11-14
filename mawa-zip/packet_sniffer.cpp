#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

FILE *logfile; // To store the output
struct sockaddr_in source, dest;
int tcp = 0, udp = 0, icmp = 0, others = 0, igmp = 0, total = 0;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);
void print_ethernet_header(const u_char *Buffer, int Size);
void print_ip_header(const u_char *Buffer, int Size);
void print_tcp_packet(const u_char *Buffer, int Size);
void print_udp_packet(const u_char *Buffer, int Size);
void print_icmp_packet(const u_char *Buffer, int Size);
void print_data(const u_char *data, int Size);

int main() {
    pcap_if_t *alldevsp, *device;
    pcap_t *handle; // Handle of the device that shall be sniffed
    char errbuf[100], *devname, devs[100][100];
    int count = 1, n;

    // Get the list of available devices
    printf("Finding available devices ... ");
    if (pcap_findalldevs(&alldevsp, errbuf)) {
        printf("Error finding devices: %s", errbuf);
        exit(1);
    }
    printf("Done\n");

    // Print the available devices
    printf("\nAvailable Devices are:\n");
    for (device = alldevsp; device != NULL; device = device->next) {
        printf("%d. %s - %s\n", count, device->name, device->description);
        if (device->name != NULL) {
            strcpy(devs[count], device->name);
        }
        count++;
    }

    // Ask user which device to sniff
    printf("Enter the number of the device you want to sniff: ");
    scanf("%d", &n);
    devname = devs[n];

    // Open the device for sniffing
    printf("Opening device %s for sniffing ... ", devname);
    handle = pcap_open_live(devname, 65535, 1, 0, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", devname, errbuf);
        exit(1);
    }
    printf("Done\n");

    logfile = fopen("log.txt", "w");
    if (logfile == NULL) {
        printf("Unable to create file.\n");
    }

    // Put the device in sniff loop
    pcap_loop(handle, 0, process_packet, NULL);

    return 0;
}

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer) {
    int size = header->len;
    struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    ++total;

    switch (iph->protocol) {
        case 1: // ICMP Protocol
            ++icmp;
            print_icmp_packet(buffer, size);
            break;

        case 2: // IGMP Protocol
            ++igmp;
            break;

        case 6: // TCP Protocol
            ++tcp;
            print_tcp_packet(buffer, size);
            break;

        case 17: // UDP Protocol
            ++udp;
            print_udp_packet(buffer, size);
            break;

        default: // Some Other Protocol like ARP etc.
            ++others;
            break;
    }
    printf("TCP: %d   UDP: %d   ICMP: %d   IGMP: %d   Others: %d   Total: %d\r", tcp, udp, icmp, igmp, others, total);
}

void print_ethernet_header(const u_char *Buffer, int Size) {
    struct ethhdr *eth = (struct ethhdr *)Buffer;
    fprintf(logfile, "\nEthernet Header\n");
    fprintf(logfile, "   |-Destination Address: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(logfile, "   |-Source Address: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    fprintf(logfile, "   |-Protocol: %u\n", (unsigned short)eth->h_proto);
}

void print_ip_header(const u_char *Buffer, int Size) {
    print_ethernet_header(Buffer, Size);

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(logfile, "\nIP Header\n");
    fprintf(logfile, "   |-IP Version: %d\n", (unsigned int)iph->version);
    fprintf(logfile, "   |-IP Header Length: %d DWORDS or %d Bytes\n", (unsigned int)iph->ihl, ((unsigned int)(iph->ihl)) * 4);
    fprintf(logfile, "   |-Type Of Service: %d\n", (unsigned int)iph->tos);
    fprintf(logfile, "   |-IP Total Length: %d Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    fprintf(logfile, "   |-Identification: %d\n", ntohs(iph->id));
    fprintf(logfile, "   |-TTL: %d\n", (unsigned int)iph->ttl);
    fprintf(logfile, "   |-Protocol: %d\n", (unsigned int)iph->protocol);
    fprintf(logfile, "   |-Checksum: %d\n", ntohs(iph->check));
    fprintf(logfile, "   |-Source IP: %s\n", inet_ntoa(source.sin_addr));
    fprintf(logfile, "   |-Destination IP: %s\n", inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct tcphdr *tcph = (struct tcphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));
    int header_size = sizeof(struct ethhdr) + iphdrlen + tcph->doff * 4;

    fprintf(logfile, "\n\n***********************TCP Packet*************************\n");
    print_ip_header(Buffer, Size);

    fprintf(logfile, "\n");
    fprintf(logfile, "TCP Header\n");
    fprintf(logfile, "   |-Source Port: %u\n", ntohs(tcph->source));
    fprintf(logfile, "   |-Destination Port: %u\n", ntohs(tcph->dest));
    fprintf(logfile, "   |-Sequence Number: %u\n", ntohl(tcph->seq));
    fprintf(logfile, "   |-Acknowledge Number: %u\n", ntohl(tcph->ack_seq));
    fprintf(logfile, "   |-Header Length: %d DWORDS or %d BYTES\n", (unsigned int)tcph->doff, (unsigned int)tcph->doff * 4);
    fprintf(logfile, "   |-Urgent Flag: %d\n", (unsigned int)tcph->urg);
    fprintf(logfile, "   |-Acknowledgement Flag: %d\n", (unsigned int)tcph->ack);
    fprintf(logfile, "   |-Push Flag: %d\n", (unsigned int)tcph->psh);
    fprintf(logfile, "   |-Reset Flag: %d\n", (unsigned int)tcph->rst);
    fprintf(logfile, "   |-Synchronise Flag: %d\n", (unsigned int)tcph->syn);
    fprintf(logfile, "   |-Finish Flag: %d\n", (unsigned int)tcph->fin);
    fprintf(logfile, "   |-Window: %d\n", ntohs(tcph->window));
    fprintf(logfile, "   |-Checksum: %d\n", ntohs(tcph->check));
    fprintf(logfile, "   |-Urgent Pointer: %d\n", tcph->urg_ptr);

    fprintf(logfile, "\nData Dump\n");
    print_data(Buffer, iphdrlen);
    print_data(Buffer + iphdrlen, tcph->doff * 4);
    print_data(Buffer + header_size, Size - header_size);

    fprintf(logfile, "\n**************************************************\n");
}

void print_udp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct udphdr *udph = (struct udphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));
    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(struct udphdr);

    fprintf(logfile, "\n\n***********************UDP Packet*************************\n");
    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nUDP Header\n");
    fprintf(logfile, "   |-Source Port: %d\n", ntohs(udph->source));
    fprintf(logfile, "   |-Destination Port: %d\n", ntohs(udph->dest));
    fprintf(logfile, "   |-UDP Length: %d\n", ntohs(udph->len));
    fprintf(logfile, "   |-UDP Checksum: %d\n", ntohs(udph->check));

    fprintf(logfile, "\nData Dump\n");
    print_data(Buffer, iphdrlen);
    print_data(Buffer + iphdrlen, sizeof(struct udphdr));
    print_data(Buffer + header_size, Size - header_size);

    fprintf(logfile, "\n**************************************************\n");
}

void print_icmp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));

    fprintf(logfile, "\n\n***********************ICMP Packet*************************\n");
    print_ip_header(Buffer, Size);

    fprintf(logfile, "\n");
    fprintf(logfile, "ICMP Header\n");
    fprintf(logfile, "   |-Type: %d\n", (unsigned int)(icmph->type));
    fprintf(logfile, "   |-Code: %d\n", (unsigned int)(icmph->code));
    fprintf(logfile, "   |-Checksum: %d\n", ntohs(icmph->checksum));

    fprintf(logfile, "\nData Dump\n");
    print_data(Buffer, iphdrlen);
    print_data(Buffer + iphdrlen, sizeof(struct icmphdr));
    print_data(Buffer + iphdrlen + sizeof(struct icmphdr), Size - sizeof(struct ethhdr) - iphdrlen - sizeof(struct icmphdr));

    fprintf(logfile, "\n**************************************************\n");
}

void print_data(const u_char *data, int Size) {
    int i, j;
    for (i = 0; i < Size; i++) {
        if (i % 16 == 0) {
            fprintf(logfile, "\n");
            fprintf(logfile, "   ");
        }
        fprintf(logfile, " %.2X", data[i]);
    }
}
