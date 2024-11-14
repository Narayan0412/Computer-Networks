#include "../exam.h"

int
main(int argc, char** argv) {
	
	int fd = open(argv[1],O_WRONLY);

	while(1) {
		char buff[1024];
		scanf("%s",buff);
		write(fd,buff,sizeof(buff));
	}

	return 0;
}
