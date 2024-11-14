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

#define PORT 8080
void hfunc(int signo){
	printf("child started\n");
}

int main(){
	struct sockaddr_in myaddr;
	signal(SIGUSR1,hfunc);
	int sfd=socket(AF_INET, SOCK_STREAM, 0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); 
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int opt=1; 																		//default=1
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	//int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	
	int b=bind(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(b<0)
		perror("connect");
	
	b=listen(sfd, 3);		//creates a connection request queue of length backlog to queue incoming connection requests
	if(b<0)
		perror("listen");
	
	//accept the teller
	int sizee=sizeof(myaddr);
	int nsfd=accept(sfd, (struct sockaddr*)&myaddr, &sizee);


	//rawsockets
	int rsfd=socket(AF_INET,SOCK_RAW,254);
    perror("socket");
    int optval=1;
    setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));//IP_HDRINCL

    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr("172.20.10.5");

    char buff1[]="AD";

    unsigned int client_len=sizeof(client);
    printf("sending");
    sendto(rsfd,buff1,strlen(buff1)+1,0,(struct sockaddr*)&client,sizeof(client));
    perror("send");
    ////////

	while(1)
	{
		char buff[100];
		recv(nsfd, buff, 60, 0);
		if(strcmp(buff, "100")==0)
			strcpy(buff, "yes");
		else
			strcpy(buff, "no");
		send(nsfd, buff, 60, 0);
	}
	return 0;
}
