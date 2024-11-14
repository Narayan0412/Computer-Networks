#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>

using namespace std;

#define MSG_QUEUE_KEY 1234  // Arbitrary key for message queue

// Define message structure
struct msgbuf {
    long mtype;      // message type (must be > 0)
    char mtext[100]; // message content
};

int main() {
    // Create a message queue (IPC_CREAT creates a new queue if it doesn't exist)
    int msgid = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666); // read/write permissions for user
    if (msgid == -1) {
        cerr << "msgget failed: " << strerror(errno) << endl;
        return 1;
    }
    cout << "Message queue created with ID: " << msgid << endl;

    // Prepare the message to be sent
    struct msgbuf message;
    message.mtype = 1;  // Set message type (1 in this case)
    strcpy(message.mtext, "Hello from the sender!");

    // Send the message to the message queue
    if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
        cerr << "msgsnd failed: " << strerror(errno) << endl;
        return 1;
    }
    cout << "Message sent: " << message.mtext << endl;

    return 0;
}
