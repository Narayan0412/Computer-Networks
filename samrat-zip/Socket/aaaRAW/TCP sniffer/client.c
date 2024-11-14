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
#include <sys/wait.h>
#include <poll.h>
#include <pthread.h>

#define PORT 8080
int main()
{
	struct sockaddr_in myaddr;
	
	myaddr.sin_family=AF_INET;								//family	(short)
	myaddr.sin_port=htons(PORT); 							//port 		(ushort)
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	
	int sfd=socket(AF_INET, SOCK_STREAM, 0);					//creating fd of socket

	int c=connect(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(c<0)
		perror("connect");
	

	char x;
	while(1)
	{

		while(read(0, &x, 1))
		{
			send(sfd, &x, 1, 0);	
			if(x=='\n')
				break;
		}

		printf("Read over\n");

		char buff[100];
		int k=recv(sfd, buff, sizeof(buff), 0);	
		buff[k]='\0';
		printf("%s\n", buff);
		
	}
	
	close(sfd);
	
	return 0;
}
