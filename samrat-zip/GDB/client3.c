#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main()
{
	mkfifo("N4",0777);
	int wfd= open("N4",O_WRONLY);
	
	int xfd=open("p3.txt",O_RDONLY);
	char ch;
        while(read(xfd,&ch,1))
        {
	      write(wfd,&ch,1);
        }
	close(wfd);
	close(xfd);
	
	
	char* buff2=(char *)malloc(sizeof(char) * (200));
	mkfifo("N5",0777);
	int rfd=open("N5",O_RDONLY);
	while(read(rfd,buff2,200)!=NULL){
		printf("%s",buff2);
	}
	close(rfd);
	return 0;
}
