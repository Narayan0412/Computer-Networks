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
	struct sockaddr_in serverAddr, clientAddr, invgAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);

	int opt = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));


	socklen_t addrSize = sizeof(struct sockaddr_in);
	int bf = bind(sfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	printf("Server's ready\n");

	char buff[100];         //, sbuff[100] = "Hello from Server!";
    //10q from MNCR
	for(int i=0; i<10; i++)
	{
        //first time recv the roll no and name
		int bytesRead = recvfrom(sfd, buff, sizeof(buff), 0, (struct sockaddr*) &clientAddr, &addrSize);
		buff[bytesRead-1] = '\0';
		printf("Received: %s\n", buff);

        strcpy(buff, "Question");
        //strcat(buff, '0'+i);
		sendto(sfd, buff, strlen(buff), 0, (struct sockaddr*) &clientAddr, addrSize);
	}

    //rec the 10th ans
    recvfrom(sfd, buff, sizeof(buff), 0, (struct sockaddr*) &clientAddr, &addrSize);

    //recv the Report from I
    int invgAddr_size=sizeof(invgAddr);
    recvfrom(sfd, buff, sizeof(buff), 0, (struct sockaddr*) &invgAddr, &invgAddr_size);
    if(strcmp(buff, "0") == 0)
    {
        strcpy(buff, "Success");
        sendto(sfd, buff, strlen(buff), 0, (struct sockaddr*) &clientAddr, addrSize);
    }
    else
    {
        strcpy(buff, "Not Owned");
        sendto(sfd, buff, strlen(buff), 0, (struct sockaddr*) &clientAddr, addrSize);
    }

	close(sfd);
	return 0;
}
