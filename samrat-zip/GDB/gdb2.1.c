#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>
#include<poll.h>

int main(){
	char ffname[2]="N0";
	for(int i=1;i<=6;i++)
	{	
		mkfifo(ffname,0777);
		ffname[1]++;
	}
	ffname[1]='0';
	
	
	int rfd[3];
	int wfd[3];
    for(int i=0; i<3; i++)
    {
		rfd[i]= open(ffname,O_RDONLY);  //0, 2, 4
		ffname[1]++;
	  	wfd[i]= open(ffname,O_WRONLY);  //1, 3, 5
		ffname[1]++;
    }
	
	struct pollfd pfds[3];
	for(int i=0;i<3;i++)
	{
		pfds[i].fd=rfd[i];
		pfds[i].events=POLLIN;
	}
	
	int mask=0;
	while(mask!=7)        //7=111
	{
	    int ret=poll(pfds,3,500);
	    for(int i=0; i<3; i++)
	    {
	        int X=1<<i;
	        if(pfds[i].revents&POLLIN && ((mask&X)==0))
	        {
		        mask=mask|X;

		        int fd=open("file1.cpp",O_WRONLY);           //empty file to store code of p.cpp
		        char ch;
		        while(read(rfd[i],&ch,1)!=0)
		        {
		              //printf("%c", ch);
			      write(fd,&ch,1);
		        }
		        close(fd);
		        //NOTE: Add null character after reading!!!! (int the file itself)
		      
		        system("g++ file1.cpp -o file1");
		        
		        int pp[2];
		        pipe(pp);
		        int c=fork();
		        if(c>0)
		        {
			        close(pp[1]);
			        //int poutfd=open("pout.txt",O_RDONLY);
			        int flag=0;
			        int outfd=open("ot.txt",O_RDONLY);
			        
			        wait(NULL);
			        char ch, ch1;
			        while(read(pp[0],&ch,1) && read(outfd,&ch1,1))
			        {       
				        //if(strcmp(buff2,buff3))
				        if(ch!=ch1)
				        {
					        flag=1;
					        break;
				        }
			        }
			        if(flag==1){
				        write(wfd[i],"FAILED",6);
			        }
			        else{
				        write(wfd[i],"PASSED",6);
			        }
			        close(outfd);
			        close(pp[0]);
			        //close(poutfd);
		        }
		        else
		        {
			        close(pp[0]);
			        //int poutfd=open("pout.txt",O_WRONLY);
			        int itfd=open("it.txt",O_RDONLY);
			        dup2(itfd,0);
			        dup2(pp[1],1);
			        char* args[]={"./file1",NULL};
			        close(pp[1]);
			        execvp(args[0],args);
			        //execlp("./file1", NULL);
		        }
		        close(rfd[i]);
		        close(wfd[i]);
	        }
	    }
	}
	return 0;
}
