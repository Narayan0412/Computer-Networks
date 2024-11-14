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
void fun(int sig, siginfo_t *si, void *others)
{
	(*x)++;
	*y=*x;
	printf("%d\n", *x);
	kill(si->si_pid, SIGUSR1);
}

int main()
{
	int shmid=shmget(1001, 1024, 0666|IPC_CREAT);
	int shmid1=shmget(1002, 1024, 0666|IPC_CREAT);
	
	x=(int*) shmat(shmid,(void*)0, 0);		//shared memory variable
	y=(int*) shmat(shmid1,(void*)0, 0);		
	
	struct sigaction sa;
    sa.sa_sigaction=fun;
    sa.sa_flags=SA_SIGINFO|SA_RESTART|SA_NOCLDSTOP;
    sigaction(SIGUSR2, &sa, NULL);
    
	*y=getpid();
	//run B before A!!!!
	
	while(*x<100)
	{
		
	}
	
	
	return 0;
}
