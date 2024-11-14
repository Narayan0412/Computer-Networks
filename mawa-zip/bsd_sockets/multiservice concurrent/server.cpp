#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client connection and service request
void *clientHandler(void *socket_desc) {
    int client_fd = *((int *)socket_desc);
    char buffer[BUFFER_SIZE];

    // Receive the client's request
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        std::cerr << "Recv failed\n";
        close(client_fd);
        return nullptr;
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received message
    std::cout << "Received request: " << buffer << std::endl;

    // Determine the service type based on the request
    if (strncmp(buffer, "ECHO", 4) == 0) {
        // Echo service
        send(client_fd, buffer + 5, bytes_received - 5, 0);  // Send back the message
    } else if (strncmp(buffer, "TIME", 4) == 0) {
        // Time service
        time_t now = time(0);
        char *dt = ctime(&now);
        send(client_fd, dt, strlen(dt), 0);
    } else if (strncmp(buffer, "REVERSE", 7) == 0) {
        // Reverse string service
        std::string str = buffer + 8;  // Remove the 'REVERSE ' part
        std::reverse(str.begin(), str.end());
        send(client_fd, str.c_str(), str.length(), 0);
    } else {
        // Invalid service request
        const char *error_message = "Invalid service requested!";
        send(client_fd, error_message, strlen(error_message), 0);
    }

    // Close the client connection
    close(client_fd);
    std::cout << "Connection with client closed\n";

    return nullptr;
}

int main() {
    // Create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
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

        // Create a new thread to handle the client request
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, clientHandler, (void *)&client_fd) < 0) {
            std::cerr << "Could not create thread\n";
            return 1;
        }

        // Detach the thread so that resources are cleaned up after the thread finishes
        pthread_detach(thread_id);
    }

    // Close the server socket (this will never be reached in this infinite loop)
    close(server_fd);
    return 0;
}
