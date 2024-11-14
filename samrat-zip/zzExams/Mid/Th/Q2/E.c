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
	char x;
	x='$';
	write(1,&x,1);
	char buff[22];
	strcpy(buff,"E is happy about that");
	while(1){
		int sizee=1;
		for(int i=0;i<22;i++){
			write(1,&buff[i],1);
		}
		x='\n';
		write(1,&x,1);
		
		while(read(0,&x,1)){
			printf("%c",x);
			if(x=='\n'){
				break;
			}
		}
		
	}
	
	return 0;
}
