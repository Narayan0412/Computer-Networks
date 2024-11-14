#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create a client socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported\n";
        return 1;
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    // Request echo service
    const char *echo_message = "ECHO Hello, Server!";
    send(client_fd, echo_message, strlen(echo_message), 0);

    // Receive the response from the server
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';  // Null-terminate the received message
    std::cout << "Server response: " << buffer << std::endl;

    // Request time service
    const char *time_request = "TIME";
    send(client_fd, time_request, strlen(time_request), 0);

    bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';
    std::cout << "Server response: " << buffer << std::endl;

    // Request reverse string service
    const char *reverse_request = "REVERSE Hello!";
    send(client_fd, reverse_request, strlen(reverse_request), 0);

    bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';
    std::cout << "Server response: " << buffer << std::endl;

    // Close the connection
    close(client_fd);
    return 0;
}
