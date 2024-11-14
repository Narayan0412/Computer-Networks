#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define PORT 8080
int main() 
{
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        exit(EXIT_FAILURE);
    }

    char buff[100];
    recv(sockfd, buff, sizeof(buff), 0);                //get question
    strcpy(buff, "Answer");
    send(sockfd, buff, strlen(buff), 0);                //send answer
    //net gets closed by I
    

    close(sockfd);
    return 0;
}