#include <iostream>
#include <unistd.h>    // For pipe(), fork(), read(), write()
#include <cstring>     // For strerror()
#include <cerrno>      // For errno

using namespace std;

int main() {
    int pipe_fd[2];  // pipe_fd[0] is for reading, pipe_fd[1] is for writing

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        cerr << "Pipe failed: " << strerror(errno) << endl;
        return 1;
    }

    // Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed: " << strerror(errno) << endl;
        return 1;
    }
    else if (pid == 0) {
        // Child process
        close(pipe_fd[1]);  // Close the write end of the pipe, since we only read in the child

        char buffer[100];
        int bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            cerr << "Read failed: " << strerror(errno) << endl;
            return 1;
        }

        buffer[bytesRead] = '\0';  // Null-terminate the string
        cout << "Child received: " << buffer << endl;

        close(pipe_fd[0]);  // Close the read end of the pipe
    }
    else {
        // Parent process
        close(pipe_fd[0]);  // Close the read end of the pipe, since we only write in the parent

        const char *message = "Hello from parent!";
        int bytesWritten = write(pipe_fd[1], message, strlen(message));
        if (bytesWritten < 0) {
            cerr << "Write failed: " << strerror(errno) << endl;
            return 1;
        }

        cout << "Parent sent: " << message << endl;

        close(pipe_fd[1]);  // Close the write end of the pipe
    }

    return 0;
}
