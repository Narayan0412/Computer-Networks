#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include<fcntl.h>
#include<string.h>
#include <sys/types.h>
#include<unistd.h>
#include<poll.h>
#include<pthread.h>
#include<sys/shm.h>
#include<signal.h>
#include<sys/ipc.h>
#define PORT 4000

int next,prev;
int sfd;
void useSfd(int signo)
{	printf("in Ci\n");
	int sfdid=shmget(10,1024,0666|IPC_CREAT);
	int *data=(int*) shmat(sfdid,(void*)0,0);
	int sfd=*data;
	
	int shmid2=shmget(12,1024,0666|IPC_CREAT);
	int *rear=(int *) shmat(shmid2,(void*)0,0);
	*rear=prev;
	
	int shmid1=shmget(14,1024,0666|IPC_CREAT);
	int *front=(int *)shmat(shmid1,(void*)0,0);
	*front=getpid();
	
	char buff[100];
	scanf("%s",buff);
	
	send(sfd, buff, sizeof(buff), 0);
	recv(sfd,buff,sizeof(buff),0);
	printf("%s\n",buff);
	
	kill(next,SIGUSR1);
}


void changeNext(int signo,siginfo_t *s,void * arg)
{
	printf("changenext\n");
	
	next=s->si_pid;
}

void changePrev(int signo,siginfo_t *s,void * arg)
{
	printf("changeprev\n");
	
	prev=s->si_pid;

}


int main()
{
	    signal(SIGUSR1,useSfd);
	    struct sigaction sa1,sa2;
	    sa1.sa_sigaction=changeNext;
	    sa2.sa_sigaction=changePrev;
	    sa1.sa_flags=sa2.sa_flags=SA_SIGINFO|SA_RESTART|SA_NOCLDSTOP;
	    sigaction(SIGUSR2,&sa1,NULL);
	    sigaction(SIGINT,&sa2,NULL);
	    int shmid2=shmget(12,1024,0666|IPC_CREAT);
	    int *rear=(int *) shmat(shmid2,(void*)0,0);
	  
	    int shmid1=shmget(14,1024,0666|IPC_CREAT);
	    int *front=(int *)shmat(shmid1,(void*)0,0);
	    prev=*rear;
	      next=*front;
	    *rear=getpid();
	    printf("%d %d %d\n",next,prev,*rear);
	    
	    kill(prev,SIGUSR2);
	    kill(next,SIGINT);
	    //kill(getpid(),SIGUSR1);
	    while(1);

}
