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
int ports[10];
int *shms[10];
int size=0;

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}

int main()
{
	struct sockaddr_in myaddr;
	int sfd=socket(AF_INET,SOCK_DGRAM,0);

	char list[200];
	strcpy(list,"");
	int childs[10];
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); 
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	
	struct pollfd pfds[2];
	pfds[0].fd=0;
	pfds[0].events=POLLIN;
	pfds[1].fd=sfd;
	pfds[1].events=POLLIN;
	
	while(1)
	{
		int ret=poll(pfds, 2, 500);
		for(int i=0; i<2; i++)
		{
			if(pfds[i].revents & POLLIN)
			{
				if(i==0)
				{
					char buff[20];
					int porta;
					scanf("%d %s", &porta, buff);
					buff[4]='\0';
					
					char buff2[5];
					strcpy(buff2, itoa(porta, 10));
					strcat(list, buff2);
					strcat(list,"\n");
					
					printf("List now: %s\n", list);
					
					int c=fork();
					int k;
					if(c==0)
					{
						char* args[]={buff, NULL};
						k=execvp(args[0], args);
						printf("Failed to exec\n");
					}
				
					int shmid=shmget(porta, 1024, 0666|IPC_CREAT);
					shms[size]=(int*) shmat(shmid, (void*)0, 0);
						
					ports[size]=porta;
					*shms[size]=0;
					
					size++;
					printf("Returning to polling\n");
				}
				else //udp
				{
					printf("Clinet came\n");
					struct sockaddr myaddr1;
					int size=10;
					char buff[5], buff2[5];
					recvfrom(sfd, buff, 20, 0, &myaddr1,&size);			//buff has clinet port
					int cport=atoi(buff);
					sendto(sfd, list, 200, 0, &myaddr1, sizeof(myaddr1));
					
					recvfrom(sfd, buff2, 20, 0, &myaddr1,&size);		//buff2 hass service port
					int sport=atoi(buff2);
					
					for(int i=0; i<size; i++)
					{
						if(ports[i]==sport)
						{
							*shms[i]=cport;
						}
					}	
					printf("%d, %d\n", sport, cport);
					printf("Clinet handeled\n");				
				}
			}
		}
	}
	return 0;
}
