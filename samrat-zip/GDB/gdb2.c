#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>
#include<poll.h>

int main()
{ 
        int n=3;
        char ffname[2]="N0";
        for(int i=0; i<n*2; i++)
        {
          mkfifo(ffname, 0777);
          ffname[1]++;
        }
        
        int fds[n*2];
        ffname[1]='0';
        for(int i=0; i<n*2; i++)
        {
          if(i%2==0)                          //0, 2, 4 for read
            fds[i]=open(ffname, O_RDONLY);
          else                                 //1, 3, 5  for write
            fds[i]=open(ffname, O_WRONLY);
            printf("opened: %s\n", ffname);
          ffname[1]++;
        }
        

        struct pollfd pfds[n];
        for(int i=0; i<n; i++)
        {
            pfds[i].fd=fds[i*2];
            pfds[i].events=POLLIN;
        }
        

        int mask=0;
        //21=10101
        while(mask!=21)
        {
               int p=poll(pfds, n, 500);
               for(int i=0; i<n; i++)
               {
                  int X=1<<(i*2);
                  //printf("%d\n", i);
                  //if(pfds[i].revents&POLLIN)
                  //{
                    //printf("mask: %d, X: %d, masknX: %d\n", mask, X, mask&X);
                  //}
                       
                  if((pfds[i].revents&POLLIN) && ((mask&X)==0))
                  {
                      mask=mask|X;
                      printf("mask: %d\n", mask);
                      
                      char* buff=(char *)malloc(sizeof(char) * (200));	
	              
	              system("mv file1.cpp file1.txt");
	              int fd=open("file1.txt", O_WRONLY);        //empty file to store code of p.cpp
	              while(read(fds[i*2],buff,200)!=NULL){
		              write(fd,buff,200);
	              }
	              
	              system("mv file1.txt file1.cpp");
	              system("g++ file1.cpp -o file1");
	              //int pp[2];
	              //pipe(pp);
	              int c=0;
	              c=fork();
	              if(c>0){
		              //close(pp[1]);
		              int poutfd=open("pout.txt",O_RDONLY);
		              char* buff2=(char *)malloc(sizeof(char) * (200));
		              char* buff3=(char *)malloc(sizeof(char) * (200));
		              int flag=0;
		              int outfd=open("ot.txt",O_RDONLY);
		              wait(NULL);
		              while(read(poutfd,buff2,200)!=NULL&&read(outfd,buff3,200)!=NULL){
			              if(strcmp(buff2,buff3)){
				              flag=1;
				              break;
			              }
		              }
		              //mkfifo("N2",0777);
		              //int wfd=open("N2",O_WRONLY);
		              if(flag==1){
			              write(fds[i+1],"FAILED\n",7);
		              }
		              else{
			              write(fds[i+1],"PASSED\n",7);
		              }
		              close(outfd);
		              close(poutfd);
		              
	              }
	              else{
		              //close(pp[0]);
		              int poutfd=open("pout.txt",O_WRONLY);
		              int itfd=open("it.txt",O_RDONLY);
		              dup2(itfd,0);
		              dup2(poutfd,1);
		              char* args[]={"./file1",NULL};
		              execvp(args[0],args);
	              }
	              //close(rfd);
	              close(fd);
	              close(fds[i+1]);
		      close(fds[i]);
		  }
               }
        }
        
	
	return 0;
}
