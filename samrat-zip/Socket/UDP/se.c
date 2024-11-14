#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define PORT 8889
int main()
{
	struct sockaddr_in serverAddr, clientAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);

	int opt = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));


	socklen_t addrSize = sizeof(struct sockaddr_in);
	int bf = bind(sfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));


	if(fork())
	{
	}
	else
	{
		char *argv[]={"./server", NULL};
		execvp(argv[0], argv);
	}

	close(sfd);
	return 0;
}
