
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


#define BUF_LEN 100

int main()
{
    int rsfd=socket(AF_INET,SOCK_RAW,25);
    if(rsfd==-1)
        perror("socket");
        
    char buf[BUF_LEN];
    struct sockaddr_in client;
    socklen_t clilen=sizeof(client);
    cout<<"receive"<<endl;
    recvfrom(rsfd,buf,BUF_LEN,0,(sockaddr*)&client,(socklen_t*)&clilen);
    perror("recv");
     struct iphdr *ip=(struct iphdr*)buf;
    cout<<(buf+(ip->ihl)*4)<<endl;

    return 0;
}
