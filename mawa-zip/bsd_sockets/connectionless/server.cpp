#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Create a UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Set up server address
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }
    std::cout << "Server listening on port " << PORT << "...\n";

    // Receive messages from the client
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(client_addr);

    while (true) {
        ssize_t n = recvfrom(server_fd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &len);
        if (n < 0) {
            std::cerr << "Receive failed\n";
            continue;
        }

        buffer[n] = '\0'; // Null-terminate the received message
        std::cout << "Received from client: " << buffer << std::endl;

        // Send a response to the client
        const char *response = "Message received!";
        sendto(server_fd, response, strlen(response), 0, (const struct sockaddr *)&client_addr, len);
    }

    close(server_fd);
    return 0;
}
