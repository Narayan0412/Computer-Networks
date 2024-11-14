#include "../exam.h"

int
main() {

	int d1 = uds_tcp_client("DOC");
	int file = open("doc.txt",O_WRONLY);

	while(1) {
		int readfd = recv_fd(d1);
		char buff[1024];
		read(readfd,buff,sizeof(buff));
		write(file,buff,sizeof(buff));
		send_fd(d1,readfd);
	}
	return 0;
}
