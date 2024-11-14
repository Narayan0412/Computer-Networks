#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>

struct msgbuffer 
{
    long mtype;
    char mtext[100];
};

void packet_sniffer() 
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //msgqueue
    struct msgbuffer msg;
    int msgid = msgget(10001, IPC_CREAT | 0666);

    //shared memory
    int shmid = shmget(1000, sizeof(int), IPC_CREAT | 0666);
    int *dispenser=(int *)shmat(shmid, NULL, 0);

    int ids[10];
    int counter=0;
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


        if(tcp_header->syn && tcp_header->ack)
        {
            ids[counter]=ntohs(tcp_header->dest);
            counter++;
            printf("Source Port: %d\n", ntohs(tcp_header->dest));
        }

        if(counter==10)
        {
            for(int i=0;i<10;i++)
            {
                int g=msgrcv(msgid, &msg, sizeof(msg), ids[i], IPC_NOWAIT);
                if(g==-1)
                {
                    printf("Forgot!!!!\n");
                    kill(*dispenser, SIGUSR1);    
                    return;                
                }
            }
            printf("All noted\n");
            counter=0;
        }
        
        // unsigned char *data = (buffer + skip_headers);
        // printf("Outgoing Payload: %s\n", data);

        //printf("\n");
    }

    close(sockfd);
}

int main() 
{
    packet_sniffer();
    return 0;
}
