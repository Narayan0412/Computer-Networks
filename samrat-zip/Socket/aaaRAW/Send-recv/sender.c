#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>

int main() 
{
    int rsfd=socket(AF_INET,SOCK_RAW,254);
    perror("socket");
    int optval=1;
    setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));//IP_HDRINCL

    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr("172.20.10.5");

    char buff[]="hello";
    //client.sin_addr.s_addr=INADDR_ANY;

    unsigned int client_len=sizeof(client);
    printf("sending");
    sendto(rsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
    perror("send");

    return 0;
}
