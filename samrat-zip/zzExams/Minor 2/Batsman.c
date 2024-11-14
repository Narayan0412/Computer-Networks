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

struct sockaddr_un myaddr;              //to bowler
struct sockaddr_un myaddr2;             //to empire
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
  
  //special lines just for UDP connections
  socket_message.msg_name=(void*)&myaddr;
  socket_message.msg_namelen=sizeof(myaddr);
  //--------------------------------------
  
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
  socket_message.msg_name=(void*)&myaddr2;
  socket_message.msg_namelen=sizeof(myaddr2);
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

char rungen(int speed, int spin)
{
	//srand(speed*spin);
	srand(time(NULL));
    int x= (rand())%40;
    printf("%d\n",x);
    return 'A'+x;
}

int flag=1;
void hfunc(int signo){
  	flag=0;
  }

#define PATH2BAT     "/home/kali/CN_MINOR_2/q4/bbat"
#define PATH2UMP      "/home/kali/CN_MINOR_2/q4/uump"   
int main()
{	
	printf("%d\n",getpid());
	int* z;
	int shmid = shmget(10002, 1024, 0666 | IPC_CREAT); //pid of batsman
    	z = (int *)shmat(shmid, (void *)0, 0);
    	*z=getpid();
	signal(SIGUSR1,hfunc);
	
	sleep(3);
	
	int* x;
	int shmid1 = shmget(10001, 1024, 0666 | IPC_CREAT);
    	x = (int *)shmat(shmid1, (void *)0, 0);
    	
	int usfd=socket(AF_UNIX, SOCK_DGRAM, 0);
	
	myaddr.sun_family=AF_UNIX;
	strcpy(myaddr.sun_path, PATH2BAT);
	unlink(PATH2BAT);
	
	int c=bind(usfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(c<0){
		perror("bind");
	}
	
	//send fielders runs
	    int rsfd=socket(AF_INET,SOCK_RAW,254);
	    int optval=1;
	setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));//IP_HDRINCL
	    
	    struct sockaddr_in client;
		client.sin_family=AF_INET;
		//client.sin_addr.s_addr=inet_addr("127.0.0.1");
		client.sin_addr.s_addr=INADDR_ANY;

		unsigned int client_len=sizeof(client);

	    //Send ball to empire
	    int usfd2=socket(AF_UNIX, SOCK_DGRAM, 0);
	    myaddr2.sun_family=AF_UNIX;
	    strcpy(myaddr2.sun_path, PATH2UMP);
	    unlink(PATH2UMP);
	    
	while(1){
		printf("waiting\n");
		while(flag);
		printf("outside\n");
		flag=1;
		int ball=recv_fd(usfd);
		printf("recieved fd %d\n",ball);

	    //Calculate runs
	    //char speed [100];
	    char some;
	    int speed=0, spin=0;
	    read(ball,&some,1);
	    while(some!=' '){
	    	speed=speed*10+(some-'0');
	    	read(ball,&some,1);
	    }
	    read(ball,&some,1);
	    while(some!='\n'){
	    	spin=spin*10+(some-'0');
	    	read(ball,&some,1);
	    }
	    printf("finished reading\n");

	    send_fd(usfd2, ball);
	    kill(*x,SIGUSR1);
	    //...
	sleep(1);
		char runs=rungen(speed, spin);
		printf("sentfd and %c\n",runs);
		char bff[2];
		bff[0]=runs;
		bff[1]='\0';
	    sendto(rsfd, bff, strlen(bff), 0, (struct sockaddr*)&client, sizeof(client));
	    //send(rsocks, &runs, 1, 0);

	}

	return 0;
}
