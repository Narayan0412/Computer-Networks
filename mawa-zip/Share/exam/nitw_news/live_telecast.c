#include "../exam.h"

int
main(int argc, char** argv){

	int port = atoi(argv[0]);
	int fd = tcp_server(port);

	while(1){
		struct sockaddr_in cli;
		socklen_t len = sizeof(cli);
		int nsfd = accept(fd,(struct sockaddr*)&cli,&len);
		char buff[1024];
		scanf("%s",buff);
		while(strcmp(buff,"done") != 0){
			send(nsfd,buff,sizeof(buff),0);
			scanf("%s",buff);
		}
	}

	return 0;
}
