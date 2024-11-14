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


int main()
{
	int sfd=socket(AF_INET,SOCK_DGRAM,0);
	int sfd1=socket(AF_INET,SOCK_DGRAM,0);
	int sfd2=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in myaddr,myaddr1;
	
	myaddr1.sin_family=AF_INET;
	myaddr1.sin_port=htons(8080); //htons(PORT);
	myaddr1.sin_addr.s_addr = INADDR_ANY;
	
	char buff[200];
	int size=10;
	sendto(sfd, buff, 20, 0, (struct sockaddr*)&myaddr1, sizeof(myaddr1));
	recvfrom(sfd,buff, 200, 0, (struct sockaddr*)&myaddr1, &size);
	
	printf("%s\nchoose the port: ",buff);
	int sport;
	scanf("%d",&sport);
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(sport); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;	
	
	inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);
	
	strcpy(buff,"ready to connect to this");
	sendto(sfd,buff,50,0,(struct sockaddr*)&myaddr,sizeof(myaddr));
	
	int c=connect(sfd2,(struct sockaddr*)&myaddr,sizeof(myaddr));
	if(c<0){\
		perror("connect");
	
	printf("connected\n");
	char buff2[50];
	strcpy(buff2,"");
	recv(sfd2,buff2,50,0);
	printf("%s\n",buff2);
	strcpy(buff2,"");
	scanf("%s",buff2);
	send(sfd2,buff2,50,0);
	strcpy(buff2,"");
	recv(sfd2,buff2,50,0);
	printf("%s",buff2);
	
	return 0;
}
