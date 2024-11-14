#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create a UDP socket
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported\n";
        return 1;
    }

    // Send message to server
    const char *message = "Hello Server!";
    sendto(client_fd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive the response from server
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(server_addr);
    ssize_t n = recvfrom(client_fd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &len);
    if (n < 0) {
        std::cerr << "Receive failed\n";
        return 1;
    }

    buffer[n] = '\0';
    std::cout << "Server response: " << buffer << std::endl;

    close(client_fd);
    return 0;
}
