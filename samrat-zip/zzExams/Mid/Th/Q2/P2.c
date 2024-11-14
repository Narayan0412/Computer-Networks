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

int main(){
	mkfifo("N",0777|IPC_CREAT);
	int ffd=open("N",O_WRONLY);
	while(1){
		char buff[50];
		strcpy(buff,"this is from S2\n");
		write(ffd,buff,50);
		scanf("%s",buff);
	}
	
	return 0;
}
