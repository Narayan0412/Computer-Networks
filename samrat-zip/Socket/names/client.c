#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
	int PORT = 3000, addrSize;
	char *serverIP = "127.0.0.1";
	//char *serverIP = "192.168.56.1";
	char rbuff[1024], sbuff[1024] = "Hello from Client!";

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sfd < 0){
		printf("Socket Error\n");
		exit(1);
	}
	
	struct sockaddr_in serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	
	int status = connect(sfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
	
	// Retrieve remote endpoint address
	getpeername(sfd, (struct sockaddr *)&serverAddr, &addrSize);
	printf("Remote endpoint address: IP = %s & Port = %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	// Retrieve local endpoint address
	getsockname(sfd, (struct sockaddr *)&clientAddr, &addrSize);
	printf("Local endpoint address: IP = %s & Port = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	
	if(status < 0)
	{
		printf("Connection Error\n");
		exit(1);
	}
	
	printf("Connected to Server!\n");
	while(1)
	{
		send(sfd, sbuff, sizeof(sbuff), 0);
		printf("Message sent to server.\n");
		
		int bytesRead = recv(sfd, rbuff, sizeof(rbuff), 0);
		printf("Message read from server- %s\n", rbuff);
	}
	
	return 0;
}
