#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int max(const int a, const int b)
{
	return (a > b) ? a : b;
}

int main() 
{
    fd_set read_fds;  // File descriptor set for reading
    int max_fd=0;       // Maximum file descriptor value

    // Add file descriptors to the set
    int n=2; 
    int fd[n];
    fd[0] = open("f1.txt", O_RDONLY);
   	fd[1] = open("f2.txt", O_RDONLY);
   	//fd[2] = fileno(popen("./w", "r"));
   	
	FD_ZERO(&read_fds);
   	for(int i=0; i<n; i++)
	{
		FD_SET(fd[i], &read_fds);
		max_fd=max(max_fd, fd[i]);						// Find the maximum file descriptor value
	}

		
    while (1) 
    { 
        // Set the timeout (NULL means wait indefinitely)
        struct timeval timeout;
        timeout.tv_sec = 3;  // 3 seconds
        timeout.tv_usec = 0;
        
           	
	   	// Initialize the read file descriptor set

        

        int result = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (result < 0) 
        {
            perror("Error in select");
            exit(1);
        } 
        else if (result == 0) 
        {
            printf("Timeout: No data available on any file descriptor.\n");
            break;
        }
        else 
        {
            for(int i=0; i<n; i++)
            {
            	
            	if (FD_ISSET(fd[i], &read_fds)) 
            	{
            		char c;
			        int k=read(fd[i], &c, 1);
			        printf("%d %d\n", k,result);
			        if(k<=0)
			        	FD_CLR(fd[i], &read_fds);
			        else
			        {
			        	printf("Data from fd[%d]:\n", i);
			        	fflush(stdout);
			        	printf("%c", c);
			        	while(read(fd[i], &c, 1))
			        	{
			        		printf("%c", c);
			        		if(c=='\n')
			        			break;
			        	}
			        }
                } 
            }   
        }
        
        printf("\n");
        sleep(2);
    }
    
    for(int i=0; i<n; i++)
        	close(fd[i]);
    
    return 0;
}

