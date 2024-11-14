#include <iostream>
#include <unistd.h>  // For fork()

using namespace std;

int main() {
    cout << "Starting the process...\n";

    // Creating a new process using fork
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        cerr << "Fork failed!" << endl;
        return 1;
    }
    else if (pid == 0) {
        // Child process
        cout << "This is the child process with PID: " << getpid() << endl;
    }
    else {
        // Parent process
        cout << "This is the parent process with PID: " << getpid() << endl;
        cout << "Created child process with PID: " << pid << endl;
    }

    cout << "Process with PID " << getpid() << " is exiting...\n";
    return 0;
}
