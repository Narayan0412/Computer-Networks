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
#include<sys/wait.h>
#include<poll.h>
#include<pthread.h>
#include <sys/un.h>
#include <sys/types.h>

struct sockaddr_un UDSmyaddr;
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
  
  //Specially for UDP sockets
  socket_message.msg_name=(void*)&UDSmyaddr;
  socket_message.msg_namelen=sizeof(UDSmyaddr);
  //-------------------------
  
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
int Suds()
{
	int usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	
	UDSmyaddr.sun_family=AF_UNIX;
	strcpy(UDSmyaddr.sun_path, SERVER_PATH);

	unlink(SERVER_PATH);
	int x=bind(usfd, (struct sockaddr *)&UDSmyaddr, sizeof(UDSmyaddr));
	if(x<0)
		perror("bind");
		
	return usfd;
}

int main()
{
	int usfd=Suds();
	
	int fd=recv_fd(usfd);
	
	printf("recieved fd %d\n", fd);
	
	char buff='A';
	int g=send(fd, &buff, 1, 0);
	//printf("%d\n", g);
	
	//recv(sfd, &x, 1, 0)

	return 0;
}
