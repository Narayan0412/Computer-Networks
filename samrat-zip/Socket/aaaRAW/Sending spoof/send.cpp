#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;


int main()
{
    int rsfd=socket(AF_INET,SOCK_RAW,25);
    perror("socket");
    int optval=1;
    setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));//IP_HDRINCL
    cout<<"opt"<<endl;
        struct sockaddr_in client;
    client.sin_family=AF_INET;
    //client.sin_addr.s_addr=inet_addr("127.0.0.1");

    char buff[]="hello";    
    client.sin_addr.s_addr=INADDR_ANY;

    //unsigned int client_len=sizeof(client);
    cout<<"sending"<<endl;
    sendto(rsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
    //sendto(rsfd,buff,strlen(buff)+1,0,NULL,0);
   
    perror("send");

}
