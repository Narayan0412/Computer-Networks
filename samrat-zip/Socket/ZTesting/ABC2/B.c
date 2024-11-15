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

#define PORT 8080

struct sockaddr_un myaddr,myaddr1;

int send_fd(int socket, int fd_to_send, int i)
 {
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  
  char a='A'+i;
  message_buffer[0] = a;
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
 
 
 
 
 
 int recv_fd(int socket,int *i)
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

//  if(message_buffer[0] != 'F')
  //{
   /* this did not originate from the above function */
   //return -1;
  //}
  *i=message_buffer[0]-'A';
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


#define SERVER_PATH     "./UDS_C"
#define SERVER_PATH2    "./UDS_B"

char services[4][10];
struct info
{
	int i;
	int sfd;
};

int usfd;

void* func(void* args)
{
	struct info* in=(struct info *)(args);
	int count=2;
	while(count--)
	{
		int nsfd=accept(in->sfd,NULL,NULL);
		int c=0;
		c=fork();
		if(c==0)
		{
			dup2(nsfd,0);
			dup2(nsfd,1);
			char* args[]={services[in->i],NULL};
			execvp(args[0],args);
		}
	}
	send_fd(usfd,in->sfd,in->i);
}

int main()
{
	strcpy(services[0],"./S1");
	strcpy(services[1],"./S2");
	strcpy(services[2],"./S3");
	strcpy(services[3],"./S4");
	
	
	usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path,SERVER_PATH);
	
	int usfd2=socket(AF_UNIX,SOCK_DGRAM,0);
	myaddr1.sun_family=AF_UNIX;
	strcpy(myaddr1.sun_path,SERVER_PATH2);
	unlink(SERVER_PATH2);
	bind(usfd2, (struct sockaddr *)&myaddr1, sizeof(myaddr1));
	
	while(1)
	{
		struct info in;
		in.sfd=recv_fd(usfd2,&in.i);
		printf("recieved fd\n");
		if(in.i==6)
			break;
		
		pthread_t tid;
		pthread_create(&tid, NULL, &func, &in);
	}
	
	send_fd(usfd,usfd,6);
	
	return 0;
}
