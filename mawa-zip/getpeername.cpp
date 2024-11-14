#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Accept a client connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        std::cerr << "Accept failed" << std::endl;
        return -1;
    }

    // Retrieve peer's information using getpeername
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    if (getpeername(client_fd, (struct sockaddr *)&peer_addr, &peer_addr_len) < 0) {
        std::cerr << "getpeername failed" << std::endl;
        return -1;
    }

    // Print the client's IP address and port
    std::cout << "Connected to client with IP: " 
              << inet_ntoa(peer_addr.sin_addr) 
              << " and port: " 
              << ntohs(peer_addr.sin_port) 
              << std::endl;

    // Close the sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
