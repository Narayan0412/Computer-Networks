#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080       // Port number to listen on
#define BUFFER_SIZE 1024 // Size of the buffer to store received data

void print_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Step 1: Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        print_error("Socket creation failed");
    }
    printf("Socket created successfully.\n");

    // Step 2: Bind socket to the port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    server_addr.sin_port = htons(PORT);      // Set port number

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        print_error("Bind failed");
    }
    printf("Socket bound to port %d.\n", PORT);

    // Step 3: Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        print_error("Listen failed");
    }
    printf("Listening for connections...\n");

    // Step 4: Accept a connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        print_error("Connection acceptance failed");
    }
    printf("Connection accepted from %s:%d.\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // Step 5: Receive data
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            print_error("Receive failed");
        } else if (bytes_received == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received (%d bytes): %s\n", bytes_received, buffer);
    }

    // Step 6: Clean up
    close(client_fd);
    close(server_fd);
    printf("Server shut down.\n");

    return 0;
}
