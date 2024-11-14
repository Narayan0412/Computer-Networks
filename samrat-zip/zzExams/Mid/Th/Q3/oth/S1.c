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

#define PORT 8081

int sfd;
struct sockaddr_in myaddr;
int opt=1;

void* func(void* args)
{
	printf("in thread\n");
	//setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	int sizee=sizeof(myaddr);
	int nsfd=accept(sfd,(struct sockaddr*)&myaddr,&sizee);
	printf("connected\n");
	char buff[50];
	strcpy(buff,"hello welcome to S1");
	send(nsfd,buff,50,0);
	recv(nsfd,buff,50,0);
	printf("%s\n",buff);
	strcpy(buff,"ok now im closing");
	send(nsfd,buff,50,0);
	close(nsfd);
	
}

void hfunc(int signo)
{
	printf("got signal\n");
	pthread_t tid;
	pthread_create(&tid,NULL,&func,NULL);
}

int main(){
	signal(SIGUSR1,hfunc);
	printf("here\n");
	sfd=socket(AF_INET,SOCK_STREAM,0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	if(b<0)
		perror("bind");
	
	printf("binded\n");
	listen(sfd,3);

	while(1){
	}
	
	return 0;
}
