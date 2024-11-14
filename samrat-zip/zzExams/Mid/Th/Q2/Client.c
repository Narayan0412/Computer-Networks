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
	
	inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);
	
	int b=connect(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	
	while(1){
		char buff[50];
		int sizee=50;
		recv(sfd,buff,sizee,0);
		if(buff[0]=='$'){
			break;
		}
		printf("%s\n",buff);
	}
	
	printf("got into E\n");
	
	while(1){
		int sizee=1;
		char x;
		while(recv(sfd,&x,sizee,0)){
			printf("%c",x);
			if(x=='\n'){
				break;
			}
		}
		while(read(0,&x,1)){
			send(sfd,&x,1,0);
			if(x=='\n'){
				break;
			}
		}
	}
	
	return 0;
}
