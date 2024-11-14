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
#include <sys/un.h>

#define ADDRESS  "./Server1"
int main()
{
	struct sockaddr_un userv_addr;
	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, ADDRESS);
   	
  	int usfd = socket(AF_UNIX, SOCK_STREAM, 0);

	int userv_len = sizeof(userv_addr);
	if(connect(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
		perror("\n connect ");
	
	int flag=1;
	do{
		char x;
		
		while(recv(usfd, &x, 1, 0))	//recieving what service sent to server
		{	
			printf("%c",x);
			if(x=='\n')
				break;
		}

		while(read(0, &x, 1))
		{
			send(usfd, &x, 1, 0);	//sending to server
			if(x=='$')
				flag=0;
			if(x=='\n')
				break;
		}
		
	}while(flag);
	
	close(usfd);
	
	return 0;
}
