#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main(){
	mkfifo("N1",0777);
	char* buff=(char *)malloc(sizeof(char) * (50));	
	int wfd= open("N1",O_WRONLY);
	int xfd=open("it.txt",O_RDONLY);
	printf("start of sending code");
	while(read(xfd,buff,50)!=NULL){
		write(wfd,buff,50);
	}
	printf("sent code");
	close(xfd);
	return 0;
}
