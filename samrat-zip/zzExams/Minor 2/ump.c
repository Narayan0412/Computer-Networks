
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
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
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


struct sockaddr_un umpaddr,bowladdr;

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
  socket_message.msg_name=(void*)&bowladdr;
 socket_message.msg_namelen=sizeof(bowladdr);
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
  socket_message.msg_name=(void*)&umpaddr;
 socket_message.msg_namelen=sizeof(umpaddr);
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
 
 #define PATH2 "/home/kali/CN_MINOR_2/q4/bbowl"
 #define PATH3 "/home/kali/CN_MINOR_2/q4/uump"
 #define PORT 8080
 int flag=1;
 void hfunc(int signo){
 	flag=0;
 }
 
 int main(){
 	signal(SIGUSR1,hfunc);
 	int* x;
	int shmid = shmget(10001, 1024, 0666 | IPC_CREAT);
    	x = (int *)shmat(shmid, (void *)0, 0);
    	*x=getpid();
    	
    sleep(3);
    	
    	int* y;
	int shmid1 = shmget(10000, 1024, 0666 | IPC_CREAT); //pid of bowler
    	y = (int *)shmat(shmid1, (void *)0, 0);
    	
    	
    	int usfd1=socket(AF_UNIX,SOCK_DGRAM,0); //to send to bowler
    	int usfd2=socket(AF_UNIX,SOCK_DGRAM,0); //to receive from batsemen
    	int sfd=socket(AF_INET,SOCK_DGRAM,0); //to receive from fielder
    	
    	bowladdr.sun_family=AF_UNIX;
	strcpy(bowladdr.sun_path,PATH2);
	
	umpaddr.sun_family=AF_UNIX;
	strcpy(umpaddr.sun_path,PATH3);
    	
    	unlink(PATH3);
	
	int xx=bind(usfd2, (struct sockaddr *)&umpaddr, sizeof(umpaddr));
	if(xx<0){
		perror("bind");
	}
	
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	int opt=1;
	setsockopt(sfd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt));
	
	xx=bind(sfd, (struct sockaddr *)&address, sizeof(address));
    	if(xx<0){
		perror("bind");
	}
	
    	while(1){
    		printf("waiting\n");
    		while(flag);
    		printf("outside\n");
    		int afd;
    		fd_set ss;
    		FD_ZERO(&ss);
    		FD_SET(usfd2,&ss);
    		struct timeval tv;
    		tv.tv_sec = 2;
		tv.tv_usec = 0;
		int retval = select(FD_SETSIZE, &ss, NULL, NULL, &tv);
		if(FD_ISSET(usfd2,&ss)){
			char buff;
			afd=recv_fd(usfd2); //receive
			printf("receivedfd\n");
			int count=4;		//number of fielders
			int flag_out=0;
			int runs=0;
			while(count--){
				recvfrom(sfd,&buff,1,0,NULL,NULL);
				if(buff=='-'){
					flag_out=1;
				}
				else{
					runs=buff-'a';
				}
			}
			if(flag_out){
				printf("out\n");
				break;
			}
			else{
				printf("batsman scored %d\n",runs);
			}
			
		}
		else{
			printf("out\n");
			break;
		}
		send_fd(usfd1,afd);
		kill(*y,SIGUSR1);
		printf("sentfd\n");
		flag=1;
    	}
    	
 	return 0;
 }
