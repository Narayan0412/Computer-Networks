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
#include <semaphore.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/shm.h>

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


#define SERVER_PATH     "./mainserver"
#define PORTP 8080      //petrol
#define PORTD 8081      //diesel
#define PORTG 8082      //gas

int usfd;                            //Unix socket
int prsfd, drsfd, grsfd;                            	//Raw sockets
int pfd, dfd, gfd;
int pamnt=100,damnt=100,gamnt=100;        //Amount of petrol, diesel and gas respectively

#define PPROTOCOL 250
#define DPROTOCOL 251
#define GPROTOCOL 252

void* petrol(void* arg)
{
    while(pamnt>0)
    {
        int cfd=accept(pfd, NULL, NULL); 
        printf("Petrol: %d\n", pamnt);

        char buff[100];
        int sizee=recv(cfd, buff, 100, 0);
        buff[sizee]='\0';

        int x=atoi(buff);
        if(x>pamnt)
        {
            printf("Not enough fuel\n");
            close(cfd);
            continue;
        }

        pamnt-=x;
        printf("Petrol left: %d\n", pamnt);
        
        send_fd(usfd, cfd);
        printf("Sent client for billing\n\n");

        close(cfd);
    }

    printf("Petrol over\n");
    close(pfd);
    //add raw socket broadcast
    prsfd=socket(AF_INET, SOCK_RAW, PPROTOCOL);
    int optval=1;
    setsockopt(prsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));

    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr("127.0.0.1");
    char buff[]="Petrol over";
    unsigned int client_len=sizeof(client);

    sendto(prsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
    perror("send");
}

void* diesel(void* arg)
{
    while(damnt>0)
    {
        int cfd=accept(dfd, NULL, NULL); 
        printf("Diesel: %d\n", damnt);
        char buff[100];
        int sizee=recv(cfd, buff, 100, 0);
        buff[sizee]='\0';

        int x=atoi(buff);
        if(x>damnt)
        {
            printf("Not enough fuel\n");
            close(cfd);
            continue;
        }

        damnt-=x;
        printf("Diesel left: %d\n", damnt);
        
        send_fd(usfd, cfd);
        printf("Sent client for billing\n\n");

        close(cfd);
    }

    printf("Diesel over\n");
    close(dfd);
    //add raw socket broadcast
    drsfd=socket(AF_INET, SOCK_RAW, DPROTOCOL);
    int optval=1;
    setsockopt(drsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));

    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr("127.0.0.1");
    char buff[]="Diesel over";
    unsigned int client_len=sizeof(client);

    sendto(drsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
    perror("send");
}

void* gas(void* arg)
{
    while(gamnt>0)
    {
        int cfd=accept(gfd, NULL, NULL); 
        printf("Gas: %d\n", gamnt);
        char buff[100];
        int sizee=recv(cfd, buff, 100, 0);
        buff[sizee]='\0';

        int x=atoi(buff);
        printf("x: %d\n", x);
        if(x>gamnt)
        {
            printf("Not enough fuel\n");
            close(cfd);
            continue;
        }

        gamnt-=x;
        printf("Gas left: %d\n", gamnt);
        
        send_fd(usfd, cfd);
        printf("Sent client for billing\n\n");

        close(cfd);
    }

    printf("Gas over\n");
    close(gfd);
    //add raw socket broadcast
    grsfd=socket(AF_INET, SOCK_RAW, GPROTOCOL);
    int optval=1;
    setsockopt(grsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));

    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=inet_addr("127.0.0.1");
    char buff[]="Gas over";
    unsigned int client_len=sizeof(client);

    sendto(grsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
    perror("send");
}

void handler(int signo)
{   
    //To terminate the program
    kill(getpid(), SIGKILL);
}

int main()
{
    //signal
    signal(SIGUSR1, handler);

    //shared memory
    int shmid = shmget(1000, sizeof(int), IPC_CREAT | 0666);
    int *me=(int *)shmat(shmid, NULL, 0);
    *me=getpid();

	sem_unlink("/my_semaphore");			//reset the semaphore

    //Unix socket
        usfd=socket(AF_UNIX,SOCK_DGRAM,0);
        if(usfd<=0){
            perror("sfd");
        }
        
        myaddr.sun_family=AF_UNIX;
        strcpy(myaddr.sun_path,SERVER_PATH);
        int sizeeee=sizeof(myaddr);
    /////////////
	
    //Defining and creating the address
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");


    //Petrol
    addr.sin_port=htons(PORTP);
    pfd=socket(AF_INET,SOCK_STREAM,0);
    bind(pfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(pfd,5);
    int opt=1;
    setsockopt(pfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));

    //Diesel
    addr.sin_port=htons(PORTD);
    dfd=socket(AF_INET,SOCK_STREAM,0);
    bind(dfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(dfd,5);
    opt=1;
    setsockopt(pfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));

    //Gas
    addr.sin_port=htons(PORTG);
    gfd=socket(AF_INET,SOCK_STREAM,0);
    bind(gfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(gfd,5);
    opt=1;
    setsockopt(pfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));


    //Creating threads for the 3 servers
    pthread_t ptid,dtid,gtid;
    pthread_create(&ptid,NULL,petrol,NULL);
    pthread_create(&dtid,NULL,diesel,NULL);
    pthread_create(&gtid,NULL,gas,NULL);

    //Waiting for the threads to finish
    pthread_join(ptid,NULL);
    pthread_join(dtid,NULL);
    pthread_join(gtid,NULL);
	
	
	
	return 0;
}

