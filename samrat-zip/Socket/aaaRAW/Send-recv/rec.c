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
    if(rsfd==-1)perror("socket");
    char buf[50];
    struct sockaddr_in client;
    int clilen=sizeof(client);

    printf("receive\n");
    int x=recvfrom(rsfd,buf,50,0,(struct sockaddr*)&client,&clilen);
    perror("recv");

    
    printf("%d\n",x);
    struct iphdr *ip;
    ip=(struct iphdr*)buf;
    printf("%s\n",(buf+(ip->ihl)*4));

    return 0;
}
