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
#include <arpa/inet.h>
#include <sys/wait.h>
#include <poll.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/msg.h>
struct sockaddr_un myaddr;


int recv_fd(int socket)
{
  int sent_fd, available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  
  //special lines just for UDP connections
  socket_message.msg_name=(void*)&myaddr;
  socket_message.msg_namelen=sizeof(myaddr);
  //--------------------------------------
  
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
     return -1;

 if(message_buffer[0] != 'F')
 {
   /* this did not originate from the above function */
     return -1;
 }

 if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
 {
   /* we did not provide enough space for the ancillary element array */
     return -1;
 }

  /* iterate ancillary elements */
 for(control_message = CMSG_FIRSTHDR(&socket_message);
     control_message != NULL;
     control_message = CMSG_NXTHDR(&socket_message, control_message))
 {
     if( (control_message->cmsg_level == SOL_SOCKET) &&
         (control_message->cmsg_type == SCM_RIGHTS) )
     {
        sent_fd = *((int *) CMSG_DATA(control_message));
        return sent_fd;
    }
}

return -1;
}

#define SERVER_PATH     "./mainserver"
int CLuds()
{
    int fd=socket(AF_UNIX, SOCK_DGRAM, 0); 
    if(fd<=0)
        perror("sfd");
    
    myaddr.sun_family=AF_UNIX;
    strcpy(myaddr.sun_path, SERVER_PATH);
    
    return fd;
}

struct msgbuffer 
{
    long mtype;
    char mtext[100];
};

int main()
{
    //Unix
    int usfd=CLuds();

    //Semaphore
    sem_t *sem;
    sem = sem_open("/my_semaphore", O_CREAT, 0666, 1);

    //Msgqueues
    int msgid = msgget(10001, IPC_CREAT | 0666);
    msgctl(msgid, IPC_RMID, NULL);
    msgid = msgget(10001, IPC_CREAT | 0666);

    struct msgbuffer msg;

    while(1)
    {
        //synchronization
        printf("Waiting for synchronization \n");
        sem_wait(sem);
            printf("Waiting for client\n");
            fflush(stdout);

            //rebind the socket
            unlink(SERVER_PATH);                   
            int x=bind(usfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
            if(x<0)
                perror("bind");

            int fd=recv_fd(usfd);
            if(fd>0) 
                printf("recieved fd\n");
            else
                printf("Error\n");


            //getting ID (peername PORT)
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            if (getpeername(fd, (struct sockaddr*)&client_addr, &addr_len) == 0); 
                printf("Peer's port: %d\n", ntohs(client_addr.sin_port));

            msg.mtype=ntohs(client_addr.sin_port);

            //add prob that he forgets 1/5 times
            int forget=rand()%5;
            if(forget!=0){                               //if he doesn't forget
                msgsnd(msgid, &msg, sizeof(msg), 0);
                printf("Noted\n");
            }
            else
                printf("Forgot\n");

            close(usfd);
            usfd=CLuds();
        sem_post(sem);
        sleep(1);
    }

    return 0;
}
