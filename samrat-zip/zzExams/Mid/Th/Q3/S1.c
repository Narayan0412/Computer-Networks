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

#define PORT 9001
int csfd, sfd;
struct sockaddr_in myaddr, servaddr;
int opt=1;

void* func(void* args)
{
	int port=*((int *)args);
	printf("in thread\n");
	
	//fix port
	printf("%d\n", port);
	myaddr.sin_port=htons(port);
	csfd=socket(AF_INET, SOCK_STREAM, 0);
	int c=connect(csfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(c<0)
		perror("connect");
	
	printf("Connected to client\n");
	
	//The talk
	char buff[50];
	strcpy(buff, "Hello, welcome to S1");
	send(csfd, buff, 50, 0);
	
	recv(csfd, buff, 50, 0);
	printf("%s\n", buff);
	
	strcpy(buff, "Ok, now im closing");
	send(csfd, buff, 50, 0);
	close(csfd);
}


int main()
{
	sfd=socket(AF_INET, SOCK_STREAM, 0);
	myaddr.sin_family=AF_INET;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Server
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int sizee=sizeof(servaddr);
	sfd=socket(AF_INET, SOCK_STREAM, 0);				//for server
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	int b=bind(sfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(b<0)
		perror("bind");

	listen(sfd, 2);
	int nsfd=accept(sfd, (struct sockaddr*)&servaddr, &sizee);
	if(nsfd<0)
		perror("accept");

	char buff[50];
	while(1)
	{
		printf("S1 listening\n");
		recv(nsfd, buff, 50, 0);
		pthread_t tid;
		int X=atoi(buff);
		pthread_create(&tid, NULL, &func, &X);
	}
	
	return 0;
}
