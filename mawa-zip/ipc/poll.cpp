#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

using namespace std;

int main() {
    // Create two file descriptors to monitor
    const char* file1 = "/tmp/file1.txt";
    const char* file2 = "/tmp/file2.txt";

    // Open files
    int fd1 = open(file1, O_RDONLY | O_CREAT, 0666);
    if (fd1 == -1) {
        cerr << "Failed to open " << file1 << ": " << strerror(errno) << endl;
        return 1;
    }

    int fd2 = open(file2, O_RDONLY | O_CREAT, 0666);
    if (fd2 == -1) {
        cerr << "Failed to open " << file2 << ": " << strerror(errno) << endl;
        close(fd1);
        return 1;
    }

    // Set up pollfd structure
    struct pollfd pfds[2];

    pfds[0].fd = fd1;   // File descriptor for the first file
    pfds[0].events = POLLIN; // Monitor for input (readability)
    
    pfds[1].fd = fd2;   // File descriptor for the second file
    pfds[1].events = POLLIN; // Monitor for input (readability)

    // Poll for events on the file descriptors
    int ret = poll(pfds, 2, -1); // -1 means infinite timeout (wait forever)

    if (ret == -1) {
        cerr << "poll() failed: " << strerror(errno) << endl;
        close(fd1);
        close(fd2);
        return 1;
    }

    // Check which file descriptor is ready
    if (pfds[0].revents & POLLIN) {
        cout << "Data is available to read from " << file1 << endl;
    }

    if (pfds[1].revents & POLLIN) {
        cout << "Data is available to read from " << file2 << endl;
    }

    // Close the file descriptors
    close(fd1);
    close(fd2);

    return 0;
}
