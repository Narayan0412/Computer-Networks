#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/time.h>

int *x, *y;
int Aid;
void fun(int sig)
{
	(*x)++;
	*y=*x;
	printf("%d\n", *x);
	signal(SIGUSR2, fun);
	kill(Aid, SIGUSR1);

}

int main()
{
	int shmid=shmget(1001, 1024, 0666|IPC_CREAT);
	int shmid1=shmget(1002, 1024, 0666|IPC_CREAT);
	
	x=(int*) shmat(shmid,(void*)0, 0);		//shared memory variable
	y=(int*) shmat(shmid1,(void*)0, 0);		
	
	signal(SIGUSR2, fun);
	*x=getpid();
	sleep(1);
	Aid=*y;
	
	
	while(*x<100)
	{
		
	}
	
	
	return 0;
}
