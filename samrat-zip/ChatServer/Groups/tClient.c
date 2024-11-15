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


char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

void* tr(void* args){
	int msgq2=*((int*)args);
	struct message msg;
	while(1){
		msgrcv(msgq2, &msg, sizeof(msg), getpid(), 0);
		printf("%s\n", msg.buffer);
	}
}

void* tw(void* args){
	int msgq1=*((int*)args);
	struct message msg2;
	while(1){
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

int main(){

    int msgq1 = msgget((key_t)10001, 0666 | IPC_CREAT);
    int msgq2 = msgget((key_t)10002, 0666 | IPC_CREAT);
	char gg[9];
	printf("Enter the string of groups you belong to: ");
	scanf("%s",gg);
	//printf("%s\n",gg);
	
	struct message init;
	init.type=getpid();
	strcpy(init.buffer,gg);
	msgsnd(msgq1, &init, sizeof(init), 0);
   	printf("send to which group? followed by message\n");
    
    pthread_t tid1,tid2;
	pthread_create(&tid1,NULL,&tr,&msgq2);
	pthread_create(&tid2,NULL,&tw,&msgq1);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	return 0;
}

