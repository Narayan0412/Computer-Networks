#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>  // For mkfifo()

using namespace std;

int main() {
    const char *fifoPath = "/tmp/myfifo";  // FIFO file path

    // Create the FIFO if it doesn't exist
    if (mkfifo(fifoPath, 0666) == -1) {
        if (errno != EEXIST) {
            cerr << "Failed to create FIFO: " << strerror(errno) << endl;
            return 1;
        }
    } else {
        cout << "FIFO created successfully!" << endl;
    }

    // Forking to simulate sender (parent) and receiver (child)
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed: " << strerror(errno) << endl;
        return 1;
    }

    if (pid == 0) {
        // Child process (Receiver)
        int fifoFd = open(fifoPath, O_RDONLY);
        if (fifoFd == -1) {
            cerr << "Failed to open FIFO for reading: " << strerror(errno) << endl;
            return 1;
        }

        char buffer[100];
        ssize_t bytesRead = read(fifoFd, buffer, sizeof(buffer) - 1);
        if (bytesRead == -1) {
            cerr << "Failed to read from FIFO: " << strerror(errno) << endl;
            return 1;
        }

        buffer[bytesRead] = '\0';  // Null-terminate the string
        cout << "Receiver received: " << buffer << endl;

        close(fifoFd);
    } else {
        // Parent process (Sender)
        int fifoFd = open(fifoPath, O_WRONLY);
        if (fifoFd == -1) {
            cerr << "Failed to open FIFO for writing: " << strerror(errno) << endl;
            return 1;
        }

        const char *message = "Hello from the sender!";
        ssize_t bytesWritten = write(fifoFd, message, strlen(message));
        if (bytesWritten == -1) {
            cerr << "Failed to write to FIFO: " << strerror(errno) << endl;
            return 1;
        }

        cout << "Sender sent: " << message << endl;

        close(fifoFd);
    }

    return 0;
}
