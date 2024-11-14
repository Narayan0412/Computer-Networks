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

struct sockaddr_un myaddr, myaddr1;
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
 

 
int recv_fd(int socket, int *i)
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
	for(control_message = CMSG_FIRSTHDR(&socket_message); control_message != NULL; 
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


#define SERVER_PATH     "./UDS_A"
#define SERVER_PATH2    "./UDS_S"
#define PORT 8080
int main()
{
	int counter=4;
	int sfd[4];
	struct pollfd pfds[5];
	int visited[5];
	for(int i=0; i<4; i++)
	{
		int sfd[i];
		struct sockaddr_in myaddr;
		sfd[i]=socket(AF_INET, SOCK_STREAM, 0);
		
		myaddr.sin_family=AF_INET;
		myaddr.sin_port=htons(PORT+i); 
		myaddr.sin_addr.s_addr = INADDR_ANY;
		
		int opt=1;
		setsockopt(sfd[i], SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
		
		int b=bind(sfd[i],(struct sockaddr*)&myaddr,sizeof(myaddr));
		if(b<0)
			perror("connect");	
		
		b=listen(sfd[i],3);
		if(b<0)
			perror("listen");
		
		pfds[i].fd=sfd[i];
		pfds[i].events=POLLIN;
		visited[i]=0;
	}
	
	//For sending service to A
	int usfd=socket(AF_UNIX, SOCK_DGRAM, 0);
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path, SERVER_PATH);
	
	//For receiving msg from C, that some service is exhausted
	int usfd2=socket(AF_UNIX, SOCK_DGRAM, 0);				
	myaddr1.sun_family=AF_UNIX;
	strcpy(myaddr1.sun_path, SERVER_PATH2);
	unlink(SERVER_PATH2);
	int b=bind(usfd2, (struct sockaddr*)&myaddr1, sizeof(myaddr1));
	printf("%d\n", b);
	
	
	pfds[4].fd=usfd2;
	pfds[4].events=POLLIN;
	visited[4]=0;
	
	//Creation of A
	int c=fork();
	if(c==0)
	{
		char* args[]={"./A",NULL};
		execvp(args[0],args);
	}
	
	printf("init done\n");
	
	
	while(counter)
	{
		int ret=poll(pfds, 5, 500);
		for(int i=0; i<5; i++)
		{
			if((pfds[i].revents&POLLIN) && visited[i]==0)
			{
				if(i==4)					//From C
				{
					int i;
					int fd=recv_fd(usfd2, &i);
					counter--;
					printf("Counter: %d\n",counter);
				}
				else
				{
					send_fd(usfd, pfds[i].fd, i);
					printf("sent fd\n");
					visited[i]=1;
				}
			}
		}
	}
	
	send_fd(usfd, usfd, 6);
	printf("All fds passed through all servers\n"); 
	
	return 0;
}
