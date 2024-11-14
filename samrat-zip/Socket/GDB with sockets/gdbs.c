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

int nsfd[10];

void* thread_handle(void* args){
	printf("inside thread\n");
	int i=*((int*)args);

	char t[10]="file0secnd";
	t[4]+=i;
	
	
	char syscalls[100]="touch ";
	strcat(syscalls,t);
	strcat(syscalls,".cpp");
	system(syscalls);
	
	/*
	strcpy(syscalls,"mv ");
	strcat(syscalls,t);
	strcat(syscalls,".cpp ");
	strcat(syscalls,t);
	strcat(syscalls,".txt");
	system(syscalls);
	*/
	
	strcpy(syscalls,t);
	strcat(syscalls,".cpp");//strcat(syscalls,".txt");
	int fd=open(syscalls,O_WRONLY); //empty file to store code of p.cpp
	
	
	char buff;
	while(recv(nsfd[i],&buff,1,0)!=NULL){
		if(buff=='$'){
			break;
		}
		write(fd,&buff,1);
		//printf("%s",buff);
	}
	printf("recieved code from %d\n",i);
	
	/*
	strcpy(syscalls,"mv ");
	strcat(syscalls,t);
	strcat(syscalls,".txt ");
	strcat(syscalls,t);
	strcat(syscalls,".cpp");
	system(syscalls);
	*/
	
	strcpy(syscalls,"g++ ");
	strcat(syscalls,t);
	strcat(syscalls,".cpp -o ");
	strcat(syscalls,t);
	system(syscalls);
	
	
	int pp[2];
	pipe(pp);
	int c=0;
	c=fork();
	if(c>0){
		close(pp[1]);
		//int poutfd=open("pout.txt",O_RDONLY);
		char buff2;
		char buff3;
		int flag=0;
		int outfd=open("ot.txt",O_RDONLY);
		wait(NULL);
		while(read(pp[0],&buff2,1)!=NULL&&read(outfd,&buff3,1)!=NULL){
			//printf("%c\n%c\n",buff2,buff3);
			if(buff2!=buff3){
				flag=1;
				break;
			}
		}
		if(flag==1){
			send(nsfd[i],"FAILED",6,0);
		}
		else{
			send(nsfd[i],"PASSED",6,0);
		}
		close(pp[0]);
		close(outfd);
		//close(poutfd);
	}
	else{
		close(pp[0]);
		//int poutfd=open("pout.txt",O_WRONLY);
		int itfd=open("it.txt",O_RDONLY);
		dup2(itfd,0);
		dup2(pp[1],1);
		
		
		strcpy(syscalls,"./");
		strcat(syscalls,t);
		char* args[]={syscalls,NULL};
		
		
		close(pp[1]);
		execvp(args[0],args);
	}
	close(nsfd[i]);
	close(fd);
	return 0;
}

int main(){
	struct sockaddr_in myaddr;
	
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(PORT); //htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	int b=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	if(b<0){
		perror("connect");
	}
	
	listen(sfd,3);
	
	pthread_t tid[10];
	int tsize=0;
	
	while(1){
		int sizee=sizeof(myaddr);
		nsfd[tsize]=accept(sfd,(struct sockaddr*)&myaddr,&sizee);
		int c=0;
		c=fork();
		if(c==0){
			pthread_create(&tid[tsize],NULL,&thread_handle,&tsize);
			pthread_join(tid[tsize],NULL);
			return 0;
		}
		else{
			tsize++;
		}
	}
	return 0;
}
