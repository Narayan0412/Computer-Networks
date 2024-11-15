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

struct sockaddr_un UDSmyaddr;
int send_fd(int socket, int fd_to_send)
{
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_name=(void*)&UDSmyaddr;
 socket_message.msg_namelen=sizeof(UDSmyaddr);
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
int CLuds()
{
	int fd=socket(AF_UNIX, SOCK_DGRAM, 0); 
	if(fd<=0)
		perror("sfd");
	
	UDSmyaddr.sun_family=AF_UNIX;
	strcpy(UDSmyaddr.sun_path, SERVER_PATH);
	
	return fd;
}

#define PORT 8080
struct sockaddr_in TCPmyaddr;
int Stcp()
{
	TCPmyaddr.sin_family=AF_INET;
	TCPmyaddr.sin_port=htons(PORT); 
	TCPmyaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int sfd=socket(AF_INET, SOCK_STREAM, 0);
	
	
	int opt=1; 																		
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	
	int b=bind(sfd, (struct sockaddr*)&TCPmyaddr, sizeof(TCPmyaddr));
	if(b<0)
		perror("connect");
	
	b=listen(sfd, 3);		
	if(b<0)
		perror("listen");

	return sfd;
}

int ACtcp(int sfd, struct sockaddr_in TCPmyaddr)
{
	int sizee=sizeof(TCPmyaddr);
	int nsfd=accept(sfd, (struct sockaddr*)&TCPmyaddr, &sizee);

	return nsfd;
}

int main(){
	
	int usfd=CLuds();
	int sfd=Stcp();
	
	int nsfd=ACtcp(sfd, TCPmyaddr);

	int x=send_fd(usfd, nsfd);
	if(x<0)
		perror("sendmsg");
	
	printf("sent fd to client\n");
	
	return 0;
}



