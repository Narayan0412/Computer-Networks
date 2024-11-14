#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 4000

int main()
{
int mySocket=socket(AF_INET,SOCK_STREAM,0),nfds;

int opt=1;
 char* hello = "Hello from server";
 char buffer[1024]={0};
if(mySocket<0)
{
	perror("error\n");
	exit(0);
}

if (setsockopt(mySocket, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
}

struct sockaddr_in address;
int addrlen = sizeof(address);
memset(&address, 0, sizeof(address));
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);
  
    // Forcefully attaching socket to the port 8080
    if (bind(mySocket, (struct sockaddr*)&address,sizeof(address))< 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(mySocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((nfds = accept(mySocket, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
    char buffer[100]={0};
    recv(nfds, buffer, 100,0);
    send(nfds, buffer, sizeof(buffer), 0);

    }
    // closing the connected socket
    close(nfds);
    // closing the listening socket
    shutdown(mySocket, SHUT_RDWR);
    return 0;

}
