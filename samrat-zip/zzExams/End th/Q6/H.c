#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 65536

void process_udp_packet(unsigned char *buffer, int size);

int sockfd;                                     //UDP raw socket
struct sockaddr_in my_addr;

int MNCR_port, OT_port;
char MNCR_ip[100], OT_ip[100];
int friends_fds[10];
int number_of_friends = 0;
int current_friend = 0;
////////////////////////////////////////////////////////////
unsigned short checksum(unsigned short *buff, int _16bitword)
{
    unsigned long sum;
    for (sum = 0; _16bitword > 0; _16bitword--)
        sum += htons(*(buff)++);            //ntohs?
        
    do
    {
        sum = ((sum >> 16) + (sum & 0xFFFF));
    }while (sum & 0xFFFF0000);

    return (~sum);
}


//thread for friends
int sfd;
void *accept_friends(void *vargp)
{
    friends_fds[number_of_friends] = accept(sfd, NULL, NULL);
    number_of_friends++;
} 

#define PORT 8080
int main() 
{
    unsigned char buffer[BUFFER_SIZE];


    //TCP socket for F
        sfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        bind(sfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        int opt = 1;
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        listen(sfd, 5);

    //Thread to keep accepting friends
    pthread_t tid;
    pthread_create(&tid, NULL, accept_friends, NULL);


    // Create a raw socket for UDP
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the address structure
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0); // Set to 0 for any port
    my_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Sniffer started...\n");

    //firest msg from OT to MNCR with roll number and name
    int size = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (size > 0) {
        struct iphdr *ip_header = (struct iphdr *)(buffer);
        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct iphdr));
        MNCR_port = ntohs(udp_header->dest);
        strcpy(MNCR_ip, inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
        //MNCR_ip = inet_ntoa(*(struct in_addr *)&(ip_header->daddr));
        OT_port = ntohs(udp_header->source);
        strcpy(OT_ip, inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
        //OT_ip = inet_ntoa(*(struct in_addr *)&(ip_header->saddr));
    }


    // Main loop to receive and process UDP packets
    while (1) {
        int size = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (size > 0) {
            process_udp_packet(buffer, size);
        }
    }

    //threaad join
    pthread_join(tid, NULL);

    // Close the socket (unreachable in this example)
    close(sockfd);
    close(sfd);

    return 0;
}

void process_udp_packet(unsigned char *buffer, int size) 
{
    // Assuming IP header is present
    struct iphdr *ip_header = (struct iphdr *)(buffer);

    // Assuming UDP header is present
    struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct iphdr));

    // Calculate data offset to get to the payload
    int data_offset = sizeof(struct iphdr) + sizeof(struct udphdr);

    //Check if UDP or not
    if(ip_header->protocol != 17)
    {
        //kill net
    }
    else
    {
        //check if the packet is from MNCR
        //check that ip and port are same
        if(strcmp(MNCR_ip, inet_ntoa(*(struct in_addr *)&(ip_header->saddr))) == 0 && udp_header->source == MNCR_port)
        {
            send(friends_fds[current_friend], buffer+data_offset, sizeof(buffer+data_offset), 0);
            
            char buf[100];
            recv(friends_fds[current_friend], buf, sizeof(buf), 0);

            //send back to MNCR
            strcpy(buffer+data_offset, buf);        //change answer
            //switch dest and src (OT->MNCR)
            ip_header->saddr = inet_addr(OT_ip);
            ip_header->daddr = inet_addr(MNCR_ip);
            udp_header->source = htons(OT_port);
            udp_header->dest = htons(MNCR_port);

            //recalc the checksum
            ip_header->check = htons(checksum((unsigned short *)(buffer), (sizeof(struct iphdr) / 2)));

            //send UDP packet
            sendto(sockfd, buffer, size, 0, (struct sockaddr*)&my_addr, sizeof(my_addr));

            current_friend++;
        }
    }
    printf("\n");
}
