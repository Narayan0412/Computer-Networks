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

struct sockaddr_un myaddr;
 
 
int recv_fd(int socket, char *c)
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
  socket_message.msg_name=(void*)&myaddr;
  socket_message.msg_namelen=sizeof(myaddr);
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   return -1;

  //if(message_buffer[0] != 'F')
  //{
   /* this did not originate from the above function */
   //return -1;
  //}
  *c=message_buffer[0];

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
#define PORT 8080
int main()
{	
	int usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path,SERVER_PATH);

	unlink(SERVER_PATH);
	int x;
	x=bind(usfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(x<0)
		perror("bind");
	
	char c;
	int fd=recv_fd(usfd, &c);
	printf("recieved %c\n", c);
	printf("recieved fd1: %d %c\n", fd, c);
	
	fd=recv_fd(usfd, &c);
	printf("recieved %c\n", c);
	printf("recieved fd2: %d %c\n", fd, c);
	
	fd=recv_fd(usfd, &c);
	printf("recieved %c\n", c);
	printf("recieved fd3: %d %c\n", fd, c);
	

	return 0;
}
