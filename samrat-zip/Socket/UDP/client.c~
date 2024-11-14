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
	struct sockaddr_in serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	socklen_t addrSize = sizeof(struct sockaddr_in);
	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);


	char rbuff[1024], sbuff[1024];
	while(1)
	{
		printf("Enter a message: ");
		fgets(sbuff, sizeof(sbuff), stdin);
		sendto(sfd, sbuff, strlen(sbuff), 0, (struct sockaddr*)&serverAddr, addrSize);

		int bytesRead = recvfrom(sfd, rbuff, sizeof(rbuff), 0, (struct sockaddr*)&serverAddr, &addrSize);
		rbuff[bytesRead] = '\0';
		printf("Received: %s\n", rbuff);
	}

	close(sfd);
	return 0;
}
