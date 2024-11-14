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

int card, usfd;
void *checks(void *tfd) 
{
    int fd = (*(int *)tfd);

    char buff[100];
    recv(fd, buff, 60, 0);
    send(card, buff, 60, 0);
    recv(card, buff, 60, 0);

    if(strcmp(buff, "yes")==0)
    {
    	send_fd(usfd, fd);
    }
    close(fd);
}

#define PORT1 8080
#define PORT2 8081
#define SERVER_PATH     "./mainserver"
int main()
{	
	usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	if(usfd<=0){
		perror("sfd");
	}
	
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path,SERVER_PATH);
	int sizeeee=sizeof(myaddr);

	
	struct sockaddr_in myaddr2;
	int sfd=socket(AF_INET, SOCK_STREAM, 0);
	myaddr2.sin_family=AF_INET;
	myaddr2.sin_port=htons(PORT1); 
	myaddr2.sin_addr.s_addr = inet_addr("127.0.0.1");

	struct sockaddr_in myaddr3;
	int sfd2=socket(AF_INET, SOCK_STREAM, 0);
	myaddr3.sin_family=AF_INET;
	myaddr3.sin_port=htons(PORT2); 
	myaddr3.sin_addr.s_addr = inet_addr("127.0.0.1");

	//connect to card
	int c=connect(sfd2, (struct sockaddr*)&myaddr3, sizeof(myaddr3));
	if(c<0)
		perror("connect");
	

	int opt=1; 																		//default=1
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	//int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
	
	int b=bind(sfd, (struct sockaddr*)&myaddr2, sizeof(myaddr2));
	if(b<0)
		perror("connect");
	
	b=listen(sfd, 3);		//creates a connection request queue of length backlog to queue incoming connection requests
	if(b<0)
		perror("listen");
	
	pthread_t threads;
	int sizee=sizeof(struct sockaddr);
	while(1)
	{
			int nsfd=accept(sfd, (struct sockaddr*)&myaddr2, &sizee); 

			pthread_create(&threads, NULL, checks, (void *)&nsfd);
	}
	
	return 0;
}



