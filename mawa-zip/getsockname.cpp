#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_fd;
    struct sockaddr_in server_addr, local_addr;
    socklen_t local_addr_len = sizeof(local_addr);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed" << std::endl;
        return -1;
    }

    // Retrieve the local address and port using getsockname
    if (getsockname(server_fd, (struct sockaddr *)&local_addr, &local_addr_len) < 0) {
        std::cerr << "getsockname failed" << std::endl;
        return -1;
    }

    // Print the local address (IP and port)
    std::cout << "Server is listening on IP: " 
              << inet_ntoa(local_addr.sin_addr) 
              << " and port: " 
              << ntohs(local_addr.sin_port) 
              << std::endl;

    // Close the socket
    close(server_fd);

    return 0;
}
