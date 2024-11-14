#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define PORTP 8080
#define PORTD 8081
#define PORTG 8082

#define PPROTOCOL 250
#define DPROTOCOL 251
#define GPROTOCOL 252

int main() 
{
    int sockfd, rsfd;
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int fuel_type;
    printf("Select fuel type:\n");
    printf("1. Petrol\n");  
    printf("2. Diesel\n");
    printf("3. CNG\n");
    scanf("%d", &fuel_type);

    if (fuel_type == 1)
    {
        rsfd=socket(AF_INET,SOCK_RAW,PPROTOCOL);
        servaddr.sin_port = htons(PORTP);
    } 
    else if (fuel_type == 2) 
    {
        rsfd=socket(AF_INET,SOCK_RAW,DPROTOCOL);
        servaddr.sin_port = htons(PORTD);
    } 
    else if (fuel_type == 3) 
    {
        rsfd=socket(AF_INET,SOCK_RAW,GPROTOCOL);
        servaddr.sin_port = htons(PORTG);
    } 
    else {
        printf("Invalid fuel type\n");
        exit(EXIT_FAILURE);
    }  

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        exit(EXIT_FAILURE);
    }

    // Send fuel amount
    printf("Enter fuel amount: ");
    char fuel_amount[100];
    scanf("%s", fuel_amount);
    send(sockfd, fuel_amount, strlen(fuel_amount), 0);


    
    //wait for broadcast
    char buf[50];
    struct sockaddr_in client;
    int clilen=sizeof(client);
    int x=recvfrom(rsfd,buf,50,0,(struct sockaddr*)&client,&clilen);
    //perror("recvfrom");

    struct iphdr *ip;
    ip=(struct iphdr*)buf;
    printf("%s\n",(buf+(ip->ihl)*4));
    
    printf("Im done\n");

    close(sockfd);
    return 0;
}