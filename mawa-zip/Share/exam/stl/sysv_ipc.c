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

/* 15 Lines */
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

/*
	TO-DETACH : shmdt(ptr);
	TO-REMOVE : shmctl(shmid,IPC_RMID,NULL);	
*/


/* 13 Lines */
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
