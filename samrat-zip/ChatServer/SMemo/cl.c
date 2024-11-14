#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<poll.h>


char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}


int main()
{
	int shmid=shmget(getpid(),1024,0666|IPC_CREAT);
	int shmid1=shmget(getpid()+1,1024,0666|IPC_CREAT);
	int temp2=shmget(10001,1024,0666|IPC_CREAT);
	
	char *shstr=(char*) shmat(shmid,(void*)0,0);		//shared memory variable
	char *shstr1=(char*) shmat(shmid1,(void*)0,0);		//shared memory variable
	char *tempstr=(char*) shmat(temp2,(void*)0,0);		//shared memory variable
	
	strcpy(tempstr,itoa(getpid(),10));
	
	struct pollfd pfds[1];
	pfds[0].fd=0;
	pfds[0].events=POLLIN;
	while(1){
		if(strcmp(shstr,"")!=0){
			printf("%s\n",shstr);
			strcpy(shstr,"");
		}
		int ret=poll(pfds,1,500);
		if(pfds[0].revents&POLLIN){
			char buff[100],temp[100];
			strcpy(buff,"C");
			strcat(buff,itoa(getpid(),10));
			strcat(buff," ");
			scanf("%s",temp);
			strcat(buff,temp);
			strcpy(shstr1,buff);
			printf("wrote\n");
		}
	}
	//printf("Data read from memory: %s\n",str);
	
	//detach from shared memory
	//shmdt(str);
	
	// destroy the shared memory
	//shmctl(shmid,IPC_RMID,NULL);
	
	return 0;
}
