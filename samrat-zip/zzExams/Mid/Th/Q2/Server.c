#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<sys/wait.h>
#include<poll.h>
#include<pthread.h>

#define PORT 8080

int nsfds[10],size=0,start=0;
void hfunc(int signo){
	printf("got signal\n");
	if(start<size){
		int c=0;
		c=fork();
		if(c==0){
			dup2(nsfds[start],0);
			dup2(nsfds[start],1);
			char* args[]={"./E",NULL};
			execvp(args[0],args);
			//exec file
		}
		printf("created an E\n");
		start++;
	}
}

int main(){
	mkfifo("N",0777);
	signal(SIGUSR1, hfunc);

	int c;
	c=fork();
	if(c==0)		//for S4
	{
		char* args[]={"./S4",NULL};
		execvp(args[0],args);
	}
	
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	b=listen(sfd,3);
	
	
	int pp[2];			//S1
	pipe(pp);
	c=fork();
	if(c==0){
		close(pp[0]);
		dup2(pp[1],1);
		char* args[]={"./S1",NULL};
		execvp(args[0],args);
	}
	close(pp[1]);
	int ppfd=pp[0];
	
	int ffd=open("N",O_RDONLY);				//S2
	
	int pofd=fileno(popen("./S3","r")); 			//S3
	
	
	struct pollfd pfds[5];
	pfds[0].fd=0;
	pfds[0].events=POLLIN;
	pfds[1].fd=sfd;
	pfds[1].events=POLLIN;
	pfds[2].fd=ppfd;
	pfds[2].events=POLLIN;
	pfds[3].fd=ffd;
	pfds[3].events=POLLIN;
	pfds[4].fd=pofd;
	pfds[4].events=POLLIN;
	
	printf("Start\n");
	while(poll(pfds,5,500))
	{
		for(int i=0; i<5; i++)
		{
			if(pfds[i].revents&POLLIN)
			{
				if(i==1)
				{
					int sizee=sizeof(myaddr);
					nsfds[size]=accept(sfd,(struct sockaddr*)&myaddr,&sizee);
					size++;
					printf("Client arrived\n");
				}
				else
				{
					char buff[50];
					read(pfds[i].fd,buff,50);
					printf("read\n");
					for(int j=start;j<size;j++)
					{
						send(nsfds[j],buff,50,0);
						printf("sent to %d\n",j);
					}
				}
			}
		}
	}
	return 0;
}
