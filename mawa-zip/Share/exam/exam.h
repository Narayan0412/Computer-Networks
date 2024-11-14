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

#define PERM IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

struct msg {
	long type;
	char data[512];
};

/*
 * returns pointer to shared memory
 */
void*
shm(int* shmid, char* ref, int size);

/*
 * returns msqid
 * just create's the msgque
 */
int
create_msgque(char *ref);

/*
 * creates upto listen syscall
 * returns sfd
 */
int
tcp_server(int port);

/*
 * returns connected fd
 * port is server's port
 */
int
tcp_client(int port); 

/*
 * generic udp socket
 * port is binded to the udp
 */
int
udp_socket(int port);

/*
 * ADDRESS is sun_path
 * returns sfd, creates upto listen
 */
int
uds_tcp_server(char* ADDRESS);

/*
 * ADDRESS is server's path
 * returns connected fd
 */
int
uds_tcp_client(char *ADDRESS);

/*
 * generic udp uds
 * returns sfd
 */
int
uds_udp(char* ADDRESS);

int 
send_fd(int socket, int fd_to_send);

int 
recv_fd(int socket);


void*
shm(int* shmid, char* ref, int size) {

	key_t key = ftok(ref,1);
	if (key == -1)
	perror("key-gen error ");

	*shmid = shmget(key,size,PERM);
	if (*shmid == -1)
	perror("shmget error ");

	void* ptr = shmat(*shmid,0,0);
	
	return ptr;
}

int
create_msgque(char *ref) {

	key_t key = ftok(ref,1);
	if (key == -1)
	perror("Key not generated ");

	int msqid = msgget(key,PERM);
	if (msqid == -1)
	perror("message queue error ");

	return msqid;
}	

int
tcp_server(int port) {

	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=port;

	if((sfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("\n bind successful ");

	listen(sfd,10);

	return sfd;
}

int
tcp_client(int port) {

	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=port;

	bzero(&serv_addr,sizeof(serv_addr));

	if((sfd = socket(AF_INET , SOCK_STREAM , 0))==-1)
	perror("\n socket");
	else printf("\n socket created successfully\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr))==-1)
	perror("\n connect : ");
	else printf("\nconnect succesful\n");

	return sfd;
}

int
udp_socket(int port) {

	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=port;

	if((sfd = socket(AF_INET,SOCK_DGRAM,0))==-1)
	perror("\n socket ");
	else printf("\n socket created successfully");

	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	perror("\n bind : ");
	else printf("bind successful\n");

	return sfd;
}

int
uds_tcp_server(char* ) {

	int  usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);

	return usfd;
}

int
uds_tcp_client(char *ADDRESS) {

	int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

  	usfd = socket(AF_UNIX, SOCK_STREAM, 0);

  	if(usfd==-1)
  	perror("\nsocket  ");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, ADDRESS);

	userv_len = sizeof(userv_addr);

	if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1)
	perror("\n connect ");
	else printf("\nconnect succesful");

	return usfd;
}

int
uds_udp(char* ADDRESS) {

	int  usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

	usfd = socket(AF_UNIX , SOCK_DGRAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");
	
	return usfd;
}

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
