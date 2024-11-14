#include "../exam.h"

int* ptr;

void
set(int turn) {
	*ptr = turn == 1 ? 2 : 1;
	return;
}

void
action(char* buff) {

	if(buff[0] >= '0' && buff[0] <= '9'){
		int port = atoi(buff);
		int live = tcp_client(port);
		while(1) {
			char data[1024];
			if(recv(live,data,sizeof(data),0) < 0)
				break;
			printf("%s\n",data);
		}
	}
	else	printf("%s\n",buff);


}

int
main(int argc, char** argv) {

	int turn = atoi(argv[1]);
	int editor_fd = tcp_client(9877);
	char buff[1024];
	int shmid;
	ptr = shm(&shmid,".",8);

	while(1) {
		recv(editor_fd,buff,sizeof(buff),0);
		if(turn == *ptr) {
			action(buff);	
			set(turn);
		}
		else
		continue;
	}

	return 0;
}
