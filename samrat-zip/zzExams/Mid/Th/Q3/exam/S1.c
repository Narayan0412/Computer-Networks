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
struct sockaddr_in myaddr;

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
	
	int shmid=shmget(PORT, 1024, 0666|IPC_CREAT);
	int *X=(int*) shmat(shmid, (void*)0, 0);	

	char buff[50];
	while(1)
	{
		printf("S1 listening on port number %d\n", PORT);
		while(*X==0)
		{
			sleep(1);
		}
		int cport=*X;
		*X=0;
		
		pthread_t tid;
		pthread_create(&tid, NULL, &func, &cport);
	}
	
	return 0;
}
