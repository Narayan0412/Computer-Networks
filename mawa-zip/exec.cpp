#include <iostream>
#include <unistd.h>  // For execv()

using namespace std;

int main() {
    // The path to the executable we want to run (ls command)
    const char *path = "/bin/ls";

    // Arguments to pass to the executable
    // First argument is the program name, the second is the directory argument for ls
    const char *const args[] = {"ls", "-l", nullptr};  // The last argument must be nullptr

    // Execute the ls command using execv
    if (execv(path, (char *const *)args) == -1) {
        // If execv fails, it returns -1, and we print the error
        cerr << "Error executing execv: " << strerror(errno) << endl;
        return 1;
    }

    // This line will never be reached because execv replaces the current process
    cout << "This will never be printed." << endl;

    return 0;
}
