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
int CLtcp()
{
	struct sockaddr_in TCPmyaddr;
	TCPmyaddr.sin_family=AF_INET;								
	TCPmyaddr.sin_port=htons(PORT); 							
	TCPmyaddr.sin_addr.s_addr=inet_addr("127.0.0.1");	
	
	int sfd=socket(AF_INET, SOCK_STREAM, 0);
	
	int c=connect(sfd, (struct sockaddr*)&TCPmyaddr, sizeof(TCPmyaddr));
	if(c<0)
		perror("connect");

	return sfd;
}

int main()
{
	int sfd=CLtcp();			//can pass port number too
	
	char x;
	recv(sfd, &x, 1, 0);
	printf("%c\n", x);
	
	return 0;
}
