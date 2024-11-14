#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
struct message 
{
	long type;
	char buff[100];
};

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}

int main()
{
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;								//family	(short)
	myaddr.sin_port=htons(PORT); 							//port 		(ushort)
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");				//struct in_addr

	int userID=getpid();	
	printf("%d\n", userID);
	
	
	//Msg queue Prep
	int msg_que = msgget((key_t)10000, 0777| IPC_CREAT);
	msgctl(msg_que, IPC_RMID, NULL);
	msg_que = msgget((key_t)10000, 0777| IPC_CREAT);		
			
	
	struct message msg;
    char x;
	while(1)
	{
		//First to be connected
		int sfd=socket(AF_INET, SOCK_STREAM, 0);
		int c=connect(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
		if(c<0)
			perror("connect");
		
		while(recv(sfd, &x, 1, 0))			//recieving what server sent
		{	
			printf("%c", x);
			if(x=='\n')
				break;
		}
	
		while(1)
		{
			while(read(0, &x, 1))				//Input
			{	
				send(sfd, &x, 1, 0);
				if(x=='\n')
					break;
			}
			
			int k=msgrcv(msg_que, &msg, sizeof(msg), 0, IPC_NOWAIT);		//Checks for request
			if(k>0)
			{
				printf("msg recv\n");
				msg.type=atoi(msg.buff);				//to who ever was requesting
				close(sfd);
				msgsnd(msg_que, &msg, sizeof(msg), 0);
				break;
			}	
			
			printf("Anything else?\n");
		}
		
		//Sends msg request to join
		msg.type=1;
		strcpy(msg.buff, itoa(userID, 10));
		msgsnd(msg_que, &msg, sizeof(msg), 1);
		printf("Getting queued\n");	
		
		//haults here for connection
		msgrcv(msg_que, &msg, sizeof(msg), userID, 0);		
		printf("We are back\n");
	}

	return 0;
}
