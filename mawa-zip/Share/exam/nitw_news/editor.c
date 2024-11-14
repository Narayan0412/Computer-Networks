#include "../exam.h"
#include <poll.h>

int sfd, n1, n2;
int usfd, d1;

/*
 * EDITOR SERV - TCP 9877
 * READER UDS SERV - READ
 */

void
handler(int signo) {
	return ;
}

void*
init(void* args) {
	
	signal(SIGUSR1,&handler);
	sfd = tcp_server(9877);
	usfd = uds_tcp_server("DOC");
	struct sockaddr_in n_addr_1, n_addr_2;
	struct sockaddr_un doc_addr;
	socklen_t n_len_1, n_len_2;
	socklen_t doc_len;

	n_len_1 = sizeof(n_addr_1);
	n_len_2 = sizeof(n_addr_2);
	doc_len = sizeof(doc_addr);

	d1 = accept(usfd,(struct sockaddr*)&doc_addr,&doc_len);
	n1 = accept(sfd,(struct sockaddr*)&n_addr_1,&n_len_1);
	n2 = accept(sfd,(struct sockaddr*)&n_addr_2,&n_len_2);

	int shmid;
	int* ptr = shm(&shmid,".",8);
	*ptr = 1;
	printf("init completed\n");
	pthread_exit(NULL);
}

void
handle_document(char* buff, int fd) {

	send_fd(d1,fd);
	printf("REDIRECTED TO DOC\n");
	recv_fd(d1);
	printf("Recieved back fd\n");
	raise(SIGUSR1);
	return;
}

void
send_to_reader(char* buff) {

	printf("sending to readers\n");
	send(n1,buff,sizeof(buff),0);
	send(n2,buff,sizeof(buff),0);
	printf("Sent\n");
}

void
read_from_reporters() {

	char buff[1024];
	
	mkfifo("REP1",0666|IPC_CREAT);
	mkfifo("REP2",0666|IPC_CREAT);
	mkfifo("REP3",0666|IPC_CREAT);
	int r1 = open("REP1",O_RDWR);
	int r2 = open("REP2",O_RDWR);
	int r3 = open("REP3",O_RDWR);

	struct pollfd pfds[3];

	pfds[0].fd = r1;
	pfds[0].events = POLLIN;
	pfds[1].fd = r2;
	pfds[1].events = POLLIN;
	pfds[2].fd = r3;
	pfds[2].events = POLLIN;

	while(1) {
		
		poll(pfds,3,-1);
		for(int i = 0; i < 3; i++)
		if(pfds[i].revents & POLLIN){
			read(pfds[i].fd, buff, sizeof(buff));
			printf("%s\n",buff);
			if(buff[0] == '/' && buff[1] == 'd') {
				handle_document(buff,pfds[i].fd);
				pause();
			}
			else
			send_to_reader(buff);
		}

	}	
	return ;
}

int
main() {

	pthread_t tid;
	pthread_create(&tid,NULL,&init,NULL);
	read_from_reporters();

	return 0;
}
