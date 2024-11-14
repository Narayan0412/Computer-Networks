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


int main()
{
	struct sockaddr_in myaddr;
	int sfd=socket(AF_INET,SOCK_DGRAM,0);
	int ssfds[10],size=0;
	char list[200];
	strcpy(list,"");
	int childs[10];
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	
	struct pollfd pfds[12];
	pfds[0].fd=0;
	pfds[0].events=POLLIN;
	pfds[1].fd=sfd;
	pfds[1].events=POLLIN;
	
	while(1)
	{
		int ret=poll(pfds, size+2, 500);
		for(int i=0;i<size+2;i++)
		{
			if(pfds[i].revents&POLLIN)
			{
				if(i==0)
				{
					printf("in\n");
					char buff[20];
					read(0,buff,20);
					strcat(list,buff);
					strcat(list,"\n");
					printf("list now: %s\n",list);
					printf("got ");
					char server[10];
					strcpy(server,"");
					int sport=0;
					int j=0;
					while(buff[j]!=' '){
						char x[2];
						x[0]=buff[j];
						x[1]='\0';
						strcat(server,x);
						j++;
					}
					j++;
					while('0'<=buff[j]&&buff[j]<='9'){
						sport=sport*10+(buff[j]-'0');
						j++;
					}
					
					printf("%s and %d\n",server,sport);
					
					
					int c=0;
					c=fork();
					if(c==0){
						char* args[]={server,NULL};
						execvp(args[0],args);
					}
					//sleep(1);
					ssfds[size]=socket(AF_INET,SOCK_DGRAM,0);
					struct sockaddr_in myaddr1;
					myaddr1.sin_family=AF_INET;
					myaddr1.sin_port=htons(sport);
					myaddr1.sin_addr.s_addr = INADDR_ANY;
					setsockopt(ssfds[size], SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
					b=bind(ssfds[size],(struct sockaddr*)&myaddr1,sizeof(myaddr1));
					if(b<0){
						perror("bind");
					}
					pfds[size+2].fd=ssfds[size];
					pfds[size+2].events=POLLIN;
					childs[size]=c;
					size++;
				}
				else if(i==1)
				{
					struct sockaddr myaddr1;
					int size=10;
					char buff[20];
					recvfrom(sfd,buff,20,0,&myaddr1,&size);
					sendto(sfd,list,200,0,&myaddr1,sizeof(myaddr1));
				}
				else
				{
					struct sockaddr myaddr1;
					int size=10;
					char buff[50];
					recvfrom(ssfds[i-2],buff,50,0,&myaddr1,&size);
					kill(childs[i-2],SIGUSR1);
					printf("signal sent %s\n",buff);
				}
			}
		}
	}
	return 0;
}
