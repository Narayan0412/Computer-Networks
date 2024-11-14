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
{	
	printf("in c1\n");
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
	    struct sockaddr_in serv_addr;
	   
	    char buffer[1024] = { 0 };
	    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	    }
	  
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(PORT);
	  
	    // Convert IPv4 and IPv6 addresses from text to binary
	    // form
	    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	    }
	    int status=0;
	    if ((status=connect(sfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	    }
	    int shmid=shmget(10,1024,0666|IPC_CREAT);
	    int *data=(int*) shmat(shmid,(void*)0,0);
	    *data=sfd;
	    next=prev=getpid();
	    printf("%d %d\n",next,prev);
	    kill(getpid(),SIGUSR1);
	    while(1);

}
