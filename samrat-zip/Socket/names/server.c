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
	char rbuff[1024], sbuff[1024] = "Hello from Server!";

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sfd < 0){
		printf("Socket Error\n");
		exit(1);
	}
	
	int option = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
	
	struct sockaddr_in serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	
	int bf = bind(sfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	
	if(bf < 0){
		printf("Bind Error\n");
		exit(1);
	}
	
	listen(sfd, 1);
	printf("Listening...\n\n");
	
	while(1)
	{
		int nsfd = accept(sfd, (struct sockaddr*)&clientAddr, &addrSize);
		
		// Retrieve local endpoint address
		getsockname(nsfd, (struct sockaddr *)&serverAddr, &addrSize);
		printf("Local endpoint address: IP = %s & Port = %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

		// Retrieve remote endpoint address
		getpeername(nsfd, (struct sockaddr *)&clientAddr, &addrSize);
		printf("Remote endpoint address: IP = %s & Port = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		int c = fork();
		if(c > 0)  //Parent Process
		{
			close(nsfd);
		}
		else  //Child Process
		{
			close(sfd);
			
			while(1)
			{
				int bytesRead = recv(nsfd, rbuff, sizeof(rbuff), 0);
				printf("Message read from client- %s\n", rbuff);
				
				send(nsfd, sbuff, sizeof(sbuff), 0);
				printf("Message sent to client.\n");
				
				sleep(1);
			}
		}
	}
	
	return 0;
}













