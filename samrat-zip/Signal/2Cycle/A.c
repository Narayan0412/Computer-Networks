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
int Bid;
void fun(int sig)
{
	(*y)++;
	*x=*y;
	printf("%d\n", *x);
	signal(SIGUSR1, fun);
	kill(Bid, SIGUSR2);
}

int main()
{
	int shmid=shmget(1001, 1024, 0666|IPC_CREAT);
	int shmid1=shmget(1002, 1024, 0666|IPC_CREAT);
	
	x=(int*) shmat(shmid,(void*)0, 0);		//shared memory variable
	y=(int*) shmat(shmid1,(void*)0, 0);		
	
	signal(SIGUSR1, fun);
	*y=getpid();
	sleep(1);
	Bid=*x;
	
	*x=5;
	sleep(1);
	kill(Bid, SIGUSR2); 
	while(*x<100)
	{
		
	}
	
	
	return 0;
}
