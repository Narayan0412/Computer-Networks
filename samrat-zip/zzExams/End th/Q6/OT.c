#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8888
int main()
{ 
    //UDP connection with MNCR
	struct sockaddr_in serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socklen_t addrSize = sizeof(struct sockaddr_in);
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);

    //Executing Invigilator
    int c=fork();
    if(c == 0)
    {
        char *path[] = {"./I", NULL};
        execvp(path[0], path);
    }

	char buff[1024];
    //Sending name and rollnumber
		printf("Enter rollnumber and name: ");
		fgets(buff, sizeof(buff), stdin);
		sendto(sfd, buff, strlen(buff), 0, (struct sockaddr*)&serverAddr, addrSize);

    //10q from MNCR
	for(int i=0; i<10; i++)
	{
		int bytesRead = recvfrom(sfd, buff, sizeof(buff), 0, (struct sockaddr*)&serverAddr, &addrSize);
		buff[bytesRead] = '\0';
		printf("Received: %s\n", buff);
	}

	close(sfd);
	return 0;
}
