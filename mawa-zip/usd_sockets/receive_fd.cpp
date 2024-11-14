#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#define SOCKET_PATH "/tmp/uds_socket"

int recv_fd(int socket) {
    int sent_fd;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

    if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
        return -1;

    if (message_buffer[0] != 'F') {
        return -1;
    }

    if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC) {
        return -1;
    }

    for (control_message = CMSG_FIRSTHDR(&socket_message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&socket_message, control_message)) {
        if ((control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS)) {
            sent_fd = *((int *)CMSG_DATA(control_message));
            return sent_fd;
        }
    }

    return -1;
}

int main() {
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        return -1;
    }

    int received_fd = recv_fd(sockfd);
    if (received_fd == -1) {
        std::cerr << "Failed to receive file descriptor" << std::endl;
        return -1;
    }

    std::cout << "Received file descriptor: " << received_fd << std::endl;

    char buffer[100];
    ssize_t bytes_read = read(received_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        std::cout << "Read from file: " << buffer << std::endl;
    } else {
        perror("read");
    }

    close(received_fd);
    close(sockfd);

    return 0;
}
