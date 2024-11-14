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
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<sys/wait.h>
#include<poll.h>
#include<pthread.h>
#include <sys/un.h>
#include <sys/types.h>

 #define PORT 8080 //for umpire

 int main()
 {
 	int rnum; //either 5,15,25,35
 	printf("enter fielder number: ");
 	scanf("%d",&rnum);
 	int sfd=socket(AF_INET,SOCK_DGRAM,0);
 	int rsfd=socket(AF_INET,SOCK_RAW,254);
 	
 	struct sockaddr_in field_addr,umpaddr;
 	
 	field_addr.sin_family = AF_INET;
	field_addr.sin_addr.s_addr = INADDR_ANY;
		
	umpaddr.sin_family = AF_INET;
	umpaddr.sin_addr.s_addr = INADDR_ANY;
	umpaddr.sin_port = htons(PORT);
	
	int opt=1;
	setsockopt(rsfd, IP_HDRINCL, SO_BROADCAST, &opt, sizeof(opt));//IP_HDRINCL
	
 	while(1){
 		char buff[50];
 		printf("waiting\n");
 		struct sockaddr_in client;
		int clilen=sizeof(client);
 		int ret=recvfrom(rsfd,buff,50,0,(struct sockaddr*)&client,&clilen);
 		if(ret<0){
 			perror("recvfrom");
 		}
 		printf("outside and %d\n",ret);
 		struct iphdr *ip;
	  	ip=(struct iphdr*)buff;
		char newbuff[50];
		strcpy(newbuff,buff+(ip->ihl)*4);
		printf("%s\n",(buff+(ip->ihl)*4));
 		int x=newbuff[0] - 'A';
 		printf("%d\n",x);
 		char factor_buff;
 		if(x%4!=0 && x%6!=0 && x-5<=rnum && rnum<=x+5){
			printf("caught\n");
			factor_buff='-';
			sendto(sfd, &factor_buff,1,0,(struct sockaddr *)&umpaddr, sizeof(umpaddr));
 		}
 		else{
 			if(x%6==0){
 				factor_buff='a'+6;
 			}
 			else{
 				factor_buff='a'+4;
 			}
 			printf("it is a %c\n",factor_buff);
 			sendto(sfd,&factor_buff,1,0,(struct sockaddr *)&umpaddr, sizeof(umpaddr));
 		}
 	}
 	return 0;
 }
