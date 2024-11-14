#include <iostream>
#include <csignal>
#include <unistd.h>
#include <cstdlib>

// Signal handler function
void signalHandler(int sig) {
    if (sig == SIGINT) {
        std::cout << "SIGINT received. Exiting...\n";
        exit(0);
    } else if (sig == SIGTERM) {
        std::cout << "SIGTERM received. Exiting...\n";
        exit(0);
    }
}

int main() {
    // Register signal handlers
    signal(SIGINT, signalHandler);  // Handle SIGINT
    signal(SIGTERM, signalHandler); // Handle SIGTERM

    // Fork a child process
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        std::cout << "Child Process: Sending SIGINT to Parent\n";
        // Sending SIGINT signal to parent process (PID of parent is getppid())
        kill(getppid(), SIGINT);  // Parent will handle SIGINT
    } else if (pid > 0) {  // Parent process
        std::cout << "Parent Process: Waiting for signals...\n";
        // Parent sends SIGTERM to itself using raise()
        raise(SIGTERM);  // Parent will handle SIGTERM
    } else {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }

    // Just in case, waiting for signals to be handled
    while (1) {
        pause(); // Wait for signals
    }

    return 0;
}
