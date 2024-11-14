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
#include <sys/uio.h>

struct sockaddr_un myaddr;
	struct sockaddr_in myaddr2;

int send_fd(int socket, int fd_to_send, char msg[1])
{
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  //char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  //message_buffer[0] = 'F';
  io_vector[0].iov_base = msg;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_name=(void*)&myaddr;
 socket_message.msg_namelen=sizeof(myaddr);
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
}


#define SERVER_PATH     "./mainserver"
#define PORT 8080
int main(){
	
	int usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	if(usfd<=0){
		perror("sfd");
	}
	
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path,SERVER_PATH);
	int sizeeee=sizeof(myaddr);

	
	int fd=open("data.txt",O_RDONLY);
	
	int x=send_fd(usfd, fd, "1");
	if(x<0)
		perror("sendmsg");
	printf("sent fd 1\n");
		
	x=send_fd(usfd, fd, "2");
	if(x<0)
		perror("sendmsg");
	printf("sent fd 2\n");
		
	x=send_fd(usfd, fd, "3");
	if(x<0)
		perror("sendmsg");
	
	printf("sent fd 3\n");
	
	return 0;
}



