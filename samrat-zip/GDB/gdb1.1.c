#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>

int main(){
	char* buff=(char *)malloc(sizeof(char) * (200));	
	mkfifo("N1",0777);
	int rfd= open("N1",O_RDONLY);
	system("mv file1.cpp file1.txt");
	int fd=open("file1.txt",O_WRONLY); //empty file to store code of p.cpp
	while(read(rfd,buff,200)!=NULL){
		write(fd,buff,200);
		printf("%s",buff);
	}
	system("mv file1.txt file1.cpp");
	system("g++ file1.cpp -o file1");
	int pp[2];
	pipe(pp);
	int c=0;
	c=fork();
	if(c>0){
		close(pp[1]);
		//int poutfd=open("pout.txt",O_RDONLY);
		char* buff2=(char *)malloc(sizeof(char) * (200));
		char* buff3=(char *)malloc(sizeof(char) * (200));
		int flag=0;
		int outfd=open("ot.txt",O_RDONLY);
		wait(NULL);
		while(read(pp[0],buff2,200)!=NULL&&read(outfd,buff3,200)!=NULL){
			if(strcmp(buff2,buff3)){
				flag=1;
				break;
			}
		}
		mkfifo("N2",0777);
		int wfd=open("N2",O_WRONLY);
		if(flag==1){
			write(wfd,"FAILED",6);
		}
		else{
			write(wfd,"PASSED",6);
		}
		close(outfd);
		//close(poutfd);
		close(wfd);
	}
	else{
		close(pp[0]);
		//int poutfd=open("pout.txt",O_WRONLY);
		int itfd=open("it.txt",O_RDONLY);
		dup2(itfd,0);
		dup2(pp[1],1);
		char* args[]={"./file1",NULL};
		close(pp[1]);
		execvp(args[0],args);
	}
	close(rfd);
	close(fd);
	return 0;
}
