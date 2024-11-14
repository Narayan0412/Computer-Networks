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
void hfunc(int signo){
	printf("child started\n");
}

int main(){
	struct sockaddr_in myaddr;
	signal(SIGUSR1,hfunc);
	int sfd=socket(AF_INET, SOCK_STREAM, 0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); 
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int opt=1; 																		//default=1
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	//int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	
	int b=bind(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(b<0)
		perror("connect");
	
	b=listen(sfd, 3);		//creates a connection request queue of length backlog to queue incoming connection requests
	if(b<0)
		perror("listen");
	
	while(1)
	{
		int sizee=sizeof(myaddr);
		int nsfd=accept(sfd, (struct sockaddr*)&myaddr, &sizee); 
		//The accept function waits if there are no connections pending, unless the socket socket has nonblocking mode set
		
		printf("connected %d\n",nsfd);
		int c=0;
		c=fork();
		if(c==0)	// one fork to take care of one client
		{ 				
			char buff;
			recv(nsfd,&buff,1,0);
			printf("got %c\n",buff);
			
			dup2(nsfd, 0);
			dup2(nsfd, 1);
			
			char file[5];
			strcpy(file, "./S0");
			file[3]+=(int)(buff-'0');
			
			char* args[]={file, NULL};
			execvp(args[0], args);
			/*								//PROXY!!!
			int pp1[2],pp2[2];
			pipe(pp1);
			pipe(pp2);
			int c1=0;
			c1=fork();
			if(c1==0){			//one fork to exec the service requested
				close(pp1[1]);
				close(pp2[0]);
				char file[5];
				//strcpy(file,"");
				strcpy(file,"./S0");
				file[3]+=(int)(buff-'0');
				dup2(pp1[0],0);
				dup2(pp2[1],1);
				char* args[]={file,NULL};
				execvp(args[0],args);
			}
			else{				//service-client interaction done here
				close(pp1[0]);
				close(pp2[1]);
				char buff2;
				int flag=1;
				do{
					printf("now reading from service\n");
					
					while(read(pp2[0],&buff2,1)){
						printf("%c",buff2);
						send(nsfd,&buff2,1,0);
						if(buff2=='\n'){
							break;
						}
					}
					printf("now reading from client\n");
					flag=1;
					while(recv(nsfd,&buff2,1,0)){
						write(pp1[1],&buff2,1);
						printf("%c",buff2);
						if(buff2=='$'){
							flag=0;
							break;
						}
						if(buff2=='\n'){
							break;
						}
					}
				}while(flag);
				kill(c1,SIGINT);
				printf("lmao ded\n");
			}
			*/
			return 0;
		}
		//else
			//close(nsfd);
	}
	return 0;
}
