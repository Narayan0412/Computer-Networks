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

void hfunc(int signo)
{
    printf("Victim signalled\n");
}

int main()
{
    signal(SIGUSR1, hfunc);
    printf("2 current pid:%d\ncurrent gid:%d %d %d %d\n", getpid(), getpgrp(), getpgid(0), getgid(), getegid());
    //egid=effective gid
    
    int shmid = shmget(10001, 1024, 0666 | IPC_CREAT);
    pid_t *x = (int *)shmat(shmid, (void *)0, 0);
    
    sleep(1);
    
    pid_t maingid = *x;
    printf("gid recieved %d\n", maingid);
    setpgid(0, maingid);
    
    printf("New pid:%d\tnew gid:%d %d %d %d\n", getpid(), getpgrp(), getpgid(0), getgid(), getegid());
    while (1)
    {

    }
    
    return 0;
}
