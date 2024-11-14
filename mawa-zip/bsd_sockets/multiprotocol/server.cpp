#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_tcp_connection(int tcp_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = read(tcp_fd, buffer, sizeof(buffer));
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "TCP Client says: " << buffer << std::endl;
        write(tcp_fd, "TCP Message received!", 21);
    }
}

void handle_udp_connection(int udp_fd, struct sockaddr_in &client_addr) {
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(client_addr);
    ssize_t n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "UDP Client says: " << buffer << std::endl;
        sendto(udp_fd, "UDP Message received!", 21, 0, (struct sockaddr *)&client_addr, len);
    }
}

int main() {
    // Create TCP socket
    int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        std::cerr << "TCP socket creation failed!" << std::endl;
        return 1;
    }

    // Create UDP socket
    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) {
        std::cerr << "UDP socket creation failed!" << std::endl;
        return 1;
    }

    // Server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the TCP socket
    if (bind(tcp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "TCP bind failed!" << std::endl;
        return 1;
    }

    // Bind the UDP socket
    if (bind(udp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "UDP bind failed!" << std::endl;
        return 1;
    }

    // Listen for TCP connections
    if (listen(tcp_fd, 5) < 0) {
        std::cerr << "TCP listen failed!" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << " for TCP and UDP..." << std::endl;

    fd_set read_fds;
    int max_fd = std::max(tcp_fd, udp_fd) + 1;

    while (true) {
        // Clear the socket set and add both TCP and UDP sockets
        FD_ZERO(&read_fds);
        FD_SET(tcp_fd, &read_fds);
        FD_SET(udp_fd, &read_fds);

        // Wait for an activity on either TCP or UDP socket
        int activity = select(max_fd, &read_fds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "select() failed!" << std::endl;
            continue;
        }

        // Check if there is activity on the TCP socket
        if (FD_ISSET(tcp_fd, &read_fds)) {
            int new_tcp_fd = accept(tcp_fd, nullptr, nullptr);
            if (new_tcp_fd < 0) {
                std::cerr << "TCP accept failed!" << std::endl;
                continue;
            }
            std::cout << "New TCP connection established..." << std::endl;
            handle_tcp_connection(new_tcp_fd);
            close(new_tcp_fd);
        }

        // Check if there is activity on the UDP socket
        if (FD_ISSET(udp_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            handle_udp_connection(udp_fd, client_addr);
        }
    }

    close(tcp_fd);
    close(udp_fd);

    return 0;
}
