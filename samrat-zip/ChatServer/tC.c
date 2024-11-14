#include<pthread.h>
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


void* tr(void* args)
{
	int msgq2=*((int*)args);
	struct message msg;
	while(1)
	{
		msgrcv(msgq2, &msg, sizeof(msg), getpid(), 0);
		printf("%s\n", msg.buffer);
	}
}

void* tw(void* args)
{
	int msgq1=*((int*)args);
	struct message msg2;
	while(1)
	{
		char ch;
		
		strcpy(msg2.buffer, "C");
		char buf[100];
		sprintf(buf, "%d", getpid());
		strcat(msg2.buffer, buf);
		strcat(msg2.buffer, ": "); 		//change C1 to pid
		
		int k=strlen(msg2.buffer);
		msg2.type=getpid(); 		
		while(read(0, &ch, 1))
		{
			if(ch=='\n')
				break;
			msg2.buffer[k++]=ch;
		}
		msg2.buffer[k]='\0';
		msgsnd(msgq1, &msg2, sizeof(msg2), 0);
	}
}

int main(){

    int msgq1 = msgget((key_t)14534, 0666 | IPC_CREAT);
    int msgq2 = msgget((key_t)14535, 0666 | IPC_CREAT);
	
	struct message init;
	init.type=getpid();
	msgsnd(msgq1, &init, sizeof(init), 0);
    
    pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, &tr, &msgq2);
	pthread_create(&tid2, NULL, &tw, &msgq1);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	return 0;
}

