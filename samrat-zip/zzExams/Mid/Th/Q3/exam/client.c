#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<sys/wait.h>
#include<poll.h>
#include<pthread.h>

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}

#define SPORT 8888
int main()
{
	int sfd=socket(AF_INET, SOCK_DGRAM, 0);		//for server
	
	struct sockaddr_in myaddr1;
	myaddr1.sin_family=AF_INET;
	myaddr1.sin_port=htons(8080); 
	myaddr1.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//Send its port to server
	char buff[5], buff2[200];
	strcpy(buff, itoa(SPORT, 10));
	int ll=sendto(sfd, buff, sizeof(buff), 0, (struct sockaddr*)&myaddr1, sizeof(myaddr1));
	
	
	int sizee=sizeof(myaddr1);
	recvfrom(sfd, buff2, 200, 0, (struct sockaddr*)&myaddr1, &sizee);				//sends list of open services
	
	printf("%s\nChoose the service: ", buff2);
	int sport;
	scanf("%d", &sport);
	strcpy(buff, itoa(sport, 10));
	sendto(sfd, buff, sizeof(buff), 0, (struct sockaddr*)&myaddr1, sizeof(myaddr1));

	
	//Creating port for accepting Servcie
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(SPORT); 
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY); ///inet_addr("127.0.0.1");	// htonl(INADDR_ANY);
	sizee=sizeof(myaddr);
	int rsfd=socket(AF_INET, SOCK_STREAM, 0);		//for service
	int opt=1;
	setsockopt(rsfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	int b=bind(rsfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
	if(b<0)
		perror("bind");

	listen(rsfd, 1);
	int nsfd=accept(rsfd, (struct sockaddr*)&myaddr, &sizee);
	if(nsfd<0)
		perror("accept");
		
	//The talk
	char buff3[50];
	recv(nsfd, buff3, 50, 0);
	printf("%s\n", buff3);
	
	scanf("%s", buff3);
	send(nsfd, buff3, 50, 0);
	
	recv(nsfd, buff3, 50, 0);
	printf("%s\n", buff3);
	
	
	return 0;
}
