#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main(){
	mkfifo("N1",0777);
	int rfd=open("N1",O_RDONLY);
	char* buff2=(char *)malloc(sizeof(char) * (50));
		printf("start of reading from gdb");
	while(read(rfd,buff2,50)!=NULL){
		printf("%s",buff2);
	}
	return 0;
}
