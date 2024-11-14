#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

void hfunc(int signo)
{
    printf("Killer signalled\n");
}

int main()
{
    signal(SIGUSR1, hfunc);
    int shmid = shmget(10001, 1024, 0666 | IPC_CREAT);
    
    int *x = (int*)shmat(shmid, (void*)0, 0);
    *x = getpgrp();
    
    printf("Killer: %d %d\n", getpid(), getpgrp());
    sleep(2);
    
    killpg(getpgrp(), SIGUSR1);
    printf("Signal Sent\n");
    while (1)
    {
    }
    return 0;
}
