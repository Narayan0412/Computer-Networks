#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}


int main()
{
	int shmid=shmget(10001,1024,0666|IPC_CREAT);
	
	char *shstr=(char*) shmat(shmid,(void*)0,0); //shared memory variable
	
	char* rshhh[10]; //recieves from here
	char* wshhh[10]; // sends here
	int pidd[10];
	int size=1;
	
	while(1){
		for(int j=1;j<size;j++){
			if(strcmp(rshhh[j],"")!=0){
				char sst[100];
				strcpy(sst,rshhh[j]);
				printf("%s",sst);
				strcpy(rshhh[j],"");
				printf("not empty in %d\n",pidd[j]);
				for(int i=1;i<size;i++){
					if(i!=j){
						printf("%d %d\n",i,pidd[i]);
						strcpy(wshhh[i],sst);
						printf("wrote\n");
						while(strcmp(wshhh[i],"")!=0){
						
						}
						printf("oppo rec\n");
					}
				}
				printf("closed");
			}
		}
		if(strcmp(shstr,"")!=0){
			pidd[size++]=atoi(shstr);
			int ss=shmget(pidd[size-1],1024,0666|IPC_CREAT);
			char *sst=(char*) shmat(ss,(void*)0,0); //shared memory variable
			int ss1=shmget(pidd[size-1]+1,1024,0666|IPC_CREAT);
			char *sst1=(char*) shmat(ss1,(void*)0,0); //shared memory variable
			wshhh[size-1]=sst;
			rshhh[size-1]=sst1;
			printf("%s assigned to %d\n",shstr,size-1);
			strcpy(shstr,"");
			printf("hello\n");
		}

	}
	//printf("Data read from memory: %s\n",str);
	
	//detach from shared memory
	//shmdt(str);
	
	// destroy the shared memory
	//shmctl(shmid,IPC_RMID,NULL);
	
	return 0;
}
