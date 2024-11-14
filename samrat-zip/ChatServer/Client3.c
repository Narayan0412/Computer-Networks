#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
	long type;
	char buffer[100];
};

int main()
{
    	int msg1_queue = msgget((key_t)14534, 0666 | IPC_CREAT);				//similar to FD
    	int msg2_queue = msgget((key_t)14535, 0666 | IPC_CREAT);

	struct pollfd pfd[1];
	pfd[0].fd = 0;				//Reading
	pfd[0].events = POLLIN;
    
	while(1)
	{
		int r=poll(pfd, 1, 100);
		struct message msg, msg2;
		int X=msgrcv(msg2_queue, &msg, sizeof(msg), 3, IPC_NOWAIT);  //flag, status
		if(X>0)
			printf("%s\n", msg.buffer);

		if(pfd[0].revents & POLLIN)
		{
			char ch;
			int k=0;
			strcpy(msg2.buffer, "Client3: ");
			k=strlen(msg2.buffer);
			
			while(read(0, &ch, 1))
			{
				if(ch=='\n')
					break;
				msg2.buffer[k++]=ch;
			}
			msg2.buffer[k]='\0';			//Very imp
			
			msg2.type=3;
			msgsnd(msg1_queue, &msg2, sizeof(msg2), 0);
		}	
	}

	return 0;
}
