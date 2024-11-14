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
#include <time.h>


int main(){
	write(1,"U are chatting with cat bot\n",28);
	char* arr[]={"meow\n","pspspsps\n","obey me human\n"};
	char buff2;
	int flag=1;
	do{
		int index=rand()%3;
		write(1,arr[index],strlen(arr[index]));
		flag=1;
		//kill(getppid(),SIGUSR1);
		while(read(0,&buff2,1)){
			if(buff2=='$'){
				flag=0;
				break;
			}
			if(buff2=='\n'){
				break;
			}
		}
	}while(flag);
	return 0;
}
