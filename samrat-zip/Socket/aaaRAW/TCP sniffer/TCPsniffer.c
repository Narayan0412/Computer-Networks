#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>

void packet_sniffer() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //int flag=0;
    while (1) 
    {
        char buffer[4096];
        ssize_t length = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);


        if (length == -1) 
        {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        struct iphdr *ip_header = (struct iphdr *)(buffer);
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + (ip_header->ihl*4));

        //no new msg
        int skip_headers=(ip_header->ihl * 4) + (tcp_header->doff * 4);
        // if(length==skip_headers )                                            
        //     continue;
        //PROBLEM: Skips handshake

        printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
        printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
        printf("Source Port: %d\n", ntohs(tcp_header->source));
        printf("Destination Port: %d\n", ntohs(tcp_header->dest));


        printf("Syn and Ack: %d %d\n", tcp_header->syn,tcp_header->ack);
        printf("Protocol: %d\n", ip_header->protocol);
        printf("Seq num: %d\n", tcp_header->seq);
        printf("Ack num: %d\n", tcp_header->ack_seq);

        //ADD: fin ack part

        
        unsigned char *data = (buffer + skip_headers);
        printf("Outgoing Payload: %s\n", data);

        printf("\n");
    }

    close(sockfd);
}

int main() {
    packet_sniffer();
    return 0;
}
