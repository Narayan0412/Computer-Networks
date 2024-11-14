#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
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


#define SERVER_PATH     "./admin"
int Suds()
{
	int usfd=socket(AF_UNIX,SOCK_DGRAM,0);
	
	UDSmyaddr.sun_family=AF_UNIX;
	strcpy(UDSmyaddr.sun_path, SERVER_PATH);

	unlink(SERVER_PATH);
	int x=bind(usfd, (struct sockaddr *)&UDSmyaddr, sizeof(UDSmyaddr));
	if(x<0)
		perror("bind");
		
	return usfd;
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

int pids[10];
char name[10][100];
int size=0;

int main()
{	
	//Msg queue
	int msgq = msgget((key_t)10001, 0666 | IPC_CREAT);
	msgctl(msgq, IPC_RMID, NULL);	
	msgq = msgget((key_t)10001, 0666 | IPC_CREAT);		
	
	printf("Msgq = %d\n", msgq);		

	int usfd=Suds();
	int sfd=Stcp();
	
	struct pollfd pfds[2];
	pfds[0].fd=usfd;
	pfds[1].fd=sfd;
	
	for(int i=0; i<2; i++)
		pfds[i].events=POLLIN;
	
	while(1)
	{
		int ret=poll(pfds, 2, 500);
	    for(int i=0; i<2; i++)
	    {
	        if(pfds[i].revents&POLLIN)
	        {
	        	if(i==0)			//UDS
	        	{
					        	
	        		char rbuff[100];
	        		socklen_t addrSize = sizeof(struct sockaddr_in);
	        		//Name
	        		int bytesRead = recvfrom(usfd, rbuff, sizeof(rbuff), 0, (struct sockaddr*) &UDSmyaddr, &addrSize);
					rbuff[bytesRead] = '\0';
					strcpy(name[size], rbuff);
					
					//PID
	        		bytesRead = recvfrom(usfd, rbuff, sizeof(rbuff), 0, (struct sockaddr*) &UDSmyaddr, &addrSize);
					rbuff[bytesRead] = '\0';
					int tid=atoi(rbuff);
					pids[size]=tid;
					
					printf("Teacher came %s %d\n", name[size], tid);	
					
					size++;
	        	}
	        	else				//TCP
	        	{
	        		int nsfd=ACtcp(sfd, TCPmyaddr);
	        		struct message msg;
	        		
	        		printf("Student came\n");
	        		
	        		//Name of teacher
	        		char buff[100];
	        		recv(nsfd, buff, sizeof(buff), 0);
	        		//printf("%s\n", buff);
	        		
	        		//Question
	        		char buff2[100];
	        		recv(nsfd, buff2, sizeof(buff2), 0);
	        		//printf("%s\n", buff2);
	        		
	        		//Msg prep
	        		int tpid;
	        		for(int i=0; i<size; i++)
	        		{
	        			if(strcmp(name[i], buff)==0)
	        			{
	        				tpid=pids[i];
	        			}
	        		}
	        		msg.type=tpid;
	        		strcpy(msg.buffer, buff2);
	        		int l=msgsnd(msgq, &msg, sizeof(msg), 0);
	       			if(l<0)
	       				perror("msgsnd");
	       			
	       			//Signal the teacher
	       			kill(tpid, SIGUSR1);
	       			
	       			
	       			//Send the fd of student
	       			sleep(1);
	       			int y=send_fd(usfd, nsfd);
	       			//printf("%d\n", y);
	       			
	       			
	       			//Do this with signal
	       			close(usfd);
	       			usfd=Suds();
	        	}
	        }
	    }
	}
	
	return 0;
}



