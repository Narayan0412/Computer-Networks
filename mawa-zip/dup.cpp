#include <iostream>
#include <unistd.h>   // For dup2()
#include <fcntl.h>    // For open(), O_CREAT, O_WRONLY
#include <cstring>    // For strlen()
#include <cerrno>     // For errno

using namespace std;

int main() {
    // Open a file to redirect stdout
    int fileDescriptor = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fileDescriptor < 0) {
        cerr << "Failed to open file: " << strerror(errno) << endl;
        return 1;
    }

    // Redirect stdout (file descriptor 1) to the file descriptor
    if (dup2(fileDescriptor, STDOUT_FILENO) < 0) {
        cerr << "Failed to duplicate file descriptor: " << strerror(errno) << endl;
        close(fileDescriptor);
        return 1;
    }

    // Now cout is redirected to "output.txt"
    cout << "This line is written to the file instead of the console." << endl;
    cout << "dup2() redirected stdout to 'output.txt'." << endl;

    // Close the file descriptor
    close(fileDescriptor);

    return 0;
}
