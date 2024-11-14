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
int main()
{
	struct sockaddr_in myaddr;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	
	b=listen(sfd,3);
	
	int c=0;
	while(1)
	{
		int sizee=sizeof(myaddr);
		int nsfd=accept(sfd, (struct sockaddr*)&myaddr, &sizee);
		if(c!=0)
			kill(c, SIGKILL);

		c=fork();
		if(c==0)
		{
			while(1)
			{
				char buff[50], x;
				strcpy(buff, "Yes bro i know bro\n");
				send(nsfd, buff, strlen(buff),0);

				while(recv(nsfd, &x, 1, 0))
				{
					if(x=='\n')
						break;
				}
				printf("Received\n");
			}
		}
	}
	return 0;
}
