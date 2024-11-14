#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Signal handler that receives additional information through siginfo_t
void signalHandler(int sig, siginfo_t *siginfo, void *context) {
    std::cout << "Signal received: " << sig << std::endl;
    std::cout << "Sender's PID: " << siginfo->si_pid << std::endl;
}

int main() {
    // Set up the sigaction structure
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;  // Tell the signal handler to receive siginfo_t
    sa.sa_sigaction = signalHandler;  // Set the custom signal handler
    sigemptyset(&sa.sa_mask);  // No signal mask

    // Register signal handler for SIGUSR1
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        std::cerr << "Error setting signal handler!" << std::endl;
        return 1;
    }

    std::cout << "Process waiting for SIGUSR1 signal..." << std::endl;

    // Fork to create a child process
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        std::cout << "Child sending SIGUSR1 to parent..." << std::endl;
        // Send SIGUSR1 signal to parent process
        kill(getppid(), SIGUSR1);
    } else if (pid > 0) {  // Parent process
        // Parent process will wait for the signal
        pause();  // Wait for the signal to be received
    } else {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }

    return 0;
}
