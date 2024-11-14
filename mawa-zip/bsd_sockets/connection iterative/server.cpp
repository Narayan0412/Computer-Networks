#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed\n";
        return 1;
    }

    // Start listening for client connections
    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }
    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept an incoming connection from a client
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Connection established with client\n";

        // Receive data from the client
        char buffer[BUFFER_SIZE];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            std::cerr << "Recv failed\n";
            close(client_fd);
            continue;
        }

        // Null-terminate the received data and print it
        buffer[bytes_received] = '\0';
        std::cout << "Received message: " << buffer << std::endl;

        // Send a response back to the client
        const char *response = "Message received";
        ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
        if (bytes_sent < 0) {
            std::cerr << "Send failed\n";
        }

        // Close the client connection
        close(client_fd);
        std::cout << "Connection with client closed\n";
    }

    // Close the server socket (this will never be reached in this infinite loop)
    close(server_fd);
    return 0;
}
