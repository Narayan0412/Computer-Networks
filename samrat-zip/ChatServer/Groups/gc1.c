#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct message {
	long type;
	char buffer[100];
};

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

int main(){

    	int msgq1 = msgget((key_t)10001, 0666 | IPC_CREAT);
    	int msgq2 = msgget((key_t)10002, 0666 | IPC_CREAT);

	struct pollfd pfd[1];
	char gg[9];
	printf("enter the string of groups you belong to: ");
	scanf("%s",gg);
	//printf("%s\n",gg);
	struct message init;
	init.type=getpid();
	strcpy(init.buffer,gg);
	msgsnd(msgq1, &init, sizeof(init), 0);
	pfd[0].fd = 0;
	pfd[0].events = POLLIN;
    	printf("send to which group? followed by message\n");
	while(1){
		int r=poll(pfd, 1, 100);
		struct message msg, msg2;
		//int X=msgrcv(msgq2, &msg, sizeof(msg), 1, IPC_NOWAIT); //getpid here
		int X=msgrcv(msgq2, &msg, sizeof(msg), getpid(), IPC_NOWAIT);
		if(X>0)
			printf("%s\n", msg.buffer);
	
		if(pfd[0].revents & POLLIN){
			char ch;
			int k=0;
			strcpy(msg2.buffer,"C");
			strcat(msg2.buffer,itoa(getpid(),10));
			strcat(msg2.buffer, ": "); //change C1 to pid
			k=strlen(msg2.buffer);
			read(0, &ch, 1);
			msg2.type=ch-'0';
			while(read(0, &ch, 1)){
				if(ch=='\n')
					break;
				msg2.buffer[k++]=ch;
			}
			msg2.buffer[k]='\0';
			msgsnd(msgq1, &msg2, sizeof(msg2), 0);
		}	
	}

	return 0;
}



