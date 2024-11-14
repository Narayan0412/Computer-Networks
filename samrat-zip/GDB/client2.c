#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main()
{	
	mkfifo("N2",0777);
	int wfd= open("N2",O_WRONLY);
	
	int xfd=open("p2.txt",O_RDONLY);
	sleep(5);
	
	char ch;
        while(read(xfd,&ch,1))
        {
	      write(wfd,&ch,1);
        }
	close(wfd);
	close(xfd);
	
	
	char* buff2=(char *)malloc(sizeof(char) * (200));
	mkfifo("N3",0777);
	int rfd=open("N3",O_RDONLY);
	while(read(rfd,buff2,200)!=NULL){
		printf("%s",buff2);
	}
	close(rfd);
	return 0;
}
