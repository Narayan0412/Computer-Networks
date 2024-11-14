#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <cerrno>

using namespace std;

#define MSG_QUEUE_KEY 1234  // Arbitrary key for message queue

// Define message structure
struct msgbuf {
    long mtype;      // message type (must be > 0)
    char mtext[100]; // message content
};

int main() {
    // Get the message queue ID (using the same key as in the sender)
    int msgid = msgget(MSG_QUEUE_KEY, 0666);  // No need to use IPC_CREAT since it's already created
    if (msgid == -1) {
        cerr << "msgget failed: " << strerror(errno) << endl;
        return 1;
    }

    // Prepare the message buffer to receive the message
    struct msgbuf message;
    
    // Receive the message from the message queue
    if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
        cerr << "msgrcv failed: " << strerror(errno) << endl;
        return 1;
    }
    
    cout << "Message received: " << message.mtext << endl;

    // Optionally, remove the message queue after use (cleanup)
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        cerr << "msgctl(IPC_RMID) failed: " << strerror(errno) << endl;
    }

    return 0;
}
