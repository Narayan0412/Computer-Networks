#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *buf1[50], *buf2[50];
	int pp1[2];
	FILE* fin, *fout;
	pipe(pp1);
	int c;
	c=fork();
	
	if(c>0)
	{
		fout=fopen("file1.txt", "r");
		close(pp1[0]);
		while (fgets(buf1, 50, fout)!= NULL) 
		{
	  		write(pp1[1], buf1, 50);
		}
		close(fout);
		
	}
	else
	{
		fin=fopen("file2.txt", "w");
		close(pp1[1]);
		while(read(pp1[0], buf2, 50)!=NULL)
		{
			fputs(buf2, fin);
		}
		close(fin);
	}
	
	
	return 0;
}
