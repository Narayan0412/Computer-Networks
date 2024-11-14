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
	
	int sizee=sizeof(myaddr);
	int nsfd=accept(sfd, (struct sockaddr*)&myaddr, &sizee); 
	printf("connected %d\n",nsfd);
		//The accept function waits if there are no connections pending, unless the socket socket has nonblocking mode set

	while(1)
	{
		char buff[200];
		char x;
		while(recv(nsfd, &x, 1, 0))	
		{	
			printf("%c",x);
			if(x=='\n')
				break;
		}
			
		strcpy(buff, "Lessgo Floppa");	
		int k=send(nsfd, buff, strlen(buff), 0);
		//printf("%d\n", k);
	}
	return 0;
}
