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
int main(){
	struct sockaddr_in myaddr;
	
	myaddr.sin_family=AF_INET;								//family	(short)
	myaddr.sin_port=htons(PORT); 							//port 		(ushort)
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);				//struct in_addr
	// =inet_addr("127.0.0.1");
	//When we don't know the IP address of our machine, we can use the special IP address INADDR_ANY
	
	/*
	htons()- makes sure that numbers are stored in memory in network byte order, which is with the most significant byte first.
	{host-to-network short}
	Many devices store numbers in little-endian format, meaning that the least significant byte comes first, threfore they require
	swapping (eg: kali in VM)
	//printf("%d\n", myaddr.sin_port);
	*/
	
	inet_pton(AF_INET, "127.0.0.1", &myaddr.sin_addr);		//receiver, sender
	/*
		convert strings to addresses(binary form) and store the result in structure (ptr, ptr)
		
		inet_ntop(AF_INET, &my_struct,ip, *ch);	
		does the reverse task, converting the addresses stored in a sockaddr_in structure into strings.
	*/
	
	
	int sfd=socket(AF_INET, SOCK_STREAM, 0);					//creaating fd of socket
	//AF_INET=> Internet address family for IPv4
	//SOCK_STREAM socket type for TCP
	//SOCK_DGRAM=> User Datagram Protocol (UDP)
	int c=connect(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(c<0)
		perror("connect");
	
	
	char buff;	
	printf("which service do you want?(0-3)\n");
	read(0,&buff,1);
	send(sfd,&buff,1,0);
	read(0,&buff,1); // \n done after entering 0
	
	printf("now chat with the bot you chose ($ to exit): \n");
	int flag=1;
	do{
		char x;
		
		printf("bot: ");
		//MSG_DONTWAIT= doesnt block the recv function, and skips if nothing is received, BUT gives an error
		while(recv(sfd, &x, 1, 0))	//recieving what service sent to server
		{	
			printf("%c",x);
			if(x=='\n')
				break;
		}

		while(read(0, &x, 1))
		{
			send(sfd, &x, 1, 0);	//sending to server
			if(x=='$')
				flag=0;
			if(x=='\n')
				break;
		}
		
	}while(flag);
	
	close(sfd);
	
	return 0;
}
