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


#define ADDRESS  "./Server1"
int main()
{
	struct sockaddr_un userv_addr;
	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	
   	int usfd = socket(AF_UNIX, SOCK_STREAM, 0);
   	
	int opt=1; 																		
	setsockopt(usfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	
	int b=bind(usfd, (struct sockaddr*)&userv_addr, sizeof(userv_addr));
	if(b<0)
		perror("connect");
	
	b=listen(usfd, 3);		
	if(b<0)
		perror("listen");
	
	int userv_len = sizeof(userv_addr);
	int nusfd=accept(usfd, (struct sockaddr*)&userv_addr, &userv_len); 		//first client
	
	int susfd= accept(usfd, (struct sockaddr*)&userv_addr, &userv_len); 	//the other server
	
	char x;
	while(read(0, &x, 1))
	{
		send(nusfd, &x, 1, 0);	//sending to client
		if(x=='\n')
			break;
	}
	
	while(recv(nusfd, &x, 1, 0))	//recieving what client sent
	{	
		printf("%c", x);
		if(x=='\n')
			break;
	}

	int l=send_fd(susfd, nusfd);
	if(l<0)
		printf("send_fd error\n");
	
	printf("I'm out\n");
	
	//close(nusfd);
	
	return 0;
}

