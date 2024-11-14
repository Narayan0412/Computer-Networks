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
int main(){
	struct sockaddr_in myaddr;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	
	inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);		//receiver, sender
	
	int c=connect(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	if(c<0){
		perror("connect");
	}
	
	char buff;	
	int xfd=open("p1.txt",O_RDONLY);
	
	printf("sleeping\n");
	sleep(5);
	printf("woke up\n");
	
	while(read(xfd,&buff,1)!=NULL){
		send(sfd,&buff,1,0);
	}
	send(sfd,"$",1,0);
	printf("writing complete\n");
	close(xfd);
	char buff2[6];
	recv(sfd,buff2,6,0);
	printf("%s",buff2);

	close(sfd);
	return 0;
}
