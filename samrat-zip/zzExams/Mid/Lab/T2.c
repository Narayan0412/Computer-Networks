#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
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


#define SERVER_PATH     "./admin"
int CLuds()
{
	int fd=socket(AF_UNIX, SOCK_DGRAM, 0); 
	if(fd<=0)
		perror("sfd");
	
	UDSmyaddr.sun_family=AF_UNIX;
	strcpy(UDSmyaddr.sun_path, SERVER_PATH);
	
	return fd;
}

int usfd;
int msgq;
struct message 
{
	long type;
	char buffer[100];
};

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
	 
void fun(int sig)
{
	printf("signal received\n");
	
	unlink(SERVER_PATH);
	int x=bind(usfd, (struct sockaddr *)&UDSmyaddr, sizeof(UDSmyaddr));
	if(x<0)
		perror("bind");

	int fd=recv_fd(usfd);	
	printf("recieved fd %d\n", fd);
	
	struct message msg;
	msgrcv(msgq, &msg, sizeof(msg), getpid(), 0);
	
	int ans=atoi(msg.buffer);
	
	ans+=2;
	
	char buff[100];
	strcpy(buff, itoa(ans, 10));
	int y=send(fd, buff, sizeof(buff), 0);
	printf("%d\n", y);
	
	printf("student handeled\n");
	
	//unbind
	close(usfd);
	usfd=CLuds();
}


int main()
{
	signal(SIGUSR1, fun);

	usfd=CLuds();
	msgq = msgget((key_t)10001, 0666 | IPC_CREAT);	
	socklen_t addrSize = sizeof(struct sockaddr_in);
	
	char sbuff[100];
	//Name
	strcpy(sbuff, "T2");
	sendto(usfd, sbuff, strlen(sbuff), 0, (struct sockaddr*) &UDSmyaddr, addrSize);
	
	//PID
	strcpy(sbuff, itoa(getpid(), 10));
	sendto(usfd, sbuff, strlen(sbuff), 0, (struct sockaddr*) &UDSmyaddr, addrSize);
	
	
	printf("%d\n", getpid());
	
	int k;
	while(1)
	{
		scanf("%d", &k);
	}

	return 0;
}
