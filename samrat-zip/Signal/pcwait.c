#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int c;
void fun(int sig)
{
	c--;
	printf("Child terminated\n");
	signal(SIGCHLD, fun);
}


int main()
{
	int n=5;
	signal(SIGCHLD, fun);
	c=n;
	for(int i=0; i<n; i++)
	{
		int x=fork();
		if(x==0)	//child;
		{
			printf("Child born\n");
			return 0;
		}
		sleep(1);
	}
	
	printf("%d\n", c);
	while(c>0)
	{
	}
	
	printf("Parent is terminated\n");
	
	return 0;
}
