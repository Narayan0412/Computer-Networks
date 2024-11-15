#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<strings.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/msg.h>

/* 32 Lines */
int 
send_fd(int socket, int fd_to_send) {
  
	struct msghdr socket_message;
  	struct iovec io_vector[1];
	struct cmsghdr *control_message = NULL;
  	char message_buffer[1];

  	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  	int available_ancillary_element_buffer_space;

  	message_buffer[0] = 'F';
  	io_vector[0].iov_base = message_buffer;
  	io_vector[0].iov_len = 1;

  	memset(&socket_message, 0, sizeof(struct msghdr));
 	socket_message.msg_iov = io_vector;
  	socket_message.msg_iovlen = 1;

  	available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  	memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  	socket_message.msg_control = ancillary_element_buffer;
  	socket_message.msg_controllen = available_ancillary_element_buffer_space;

  	control_message = CMSG_FIRSTHDR(&socket_message);
 	control_message->cmsg_level = SOL_SOCKET;
 	control_message->cmsg_type = SCM_RIGHTS;
	control_message->cmsg_len = CMSG_LEN(sizeof(int));
 	*((int *) CMSG_DATA(control_message)) = fd_to_send;

 	return sendmsg(socket, &socket_message, 0);
}

/* 42 Lines */
int 
recv_fd(int socket) {

  	int sent_fd;
  	struct msghdr socket_message;
  	struct iovec io_vector[1];
  	struct cmsghdr *control_message = NULL;
  	char message_buffer[1];
  	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  	memset(&socket_message, 0, sizeof(struct msghdr));
  	memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  	io_vector[0].iov_base = message_buffer;
 	io_vector[0].iov_len = 1;
  	socket_message.msg_iov = io_vector;
  	socket_message.msg_iovlen = 1;

  	socket_message.msg_control = ancillary_element_buffer;
  	socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  	if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   	return -1;

  	if(message_buffer[0] != 'F')
   	return -1;

  	if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
	return -1;

   	for(control_message = CMSG_FIRSTHDR(&socket_message);
       	control_message != NULL;
       	control_message = CMSG_NXTHDR(&socket_message, control_message)) {
   		if( (control_message->cmsg_level == SOL_SOCKET) &&
       		(control_message->cmsg_type == SCM_RIGHTS) ) {
    			sent_fd = *((int *) CMSG_DATA(control_message));
    			return sent_fd;
   		}
  	}

  	return -1;
}

