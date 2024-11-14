#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct message {
	long type;
	char buffer[100];
};

int main()
{
	int n=3; 		//Number of clients	

	int main_queue = msgget((key_t)14534, 0666 | IPC_CREAT);				//msgget=creates msg queue
	int back_queue = msgget((key_t)14535, 0666 | IPC_CREAT);

	msgctl(main_queue, IPC_RMID, NULL);										//Cleans
	msgctl(back_queue, IPC_RMID, NULL);
	
	main_queue = msgget((key_t)14534, 0666 | IPC_CREAT);				
	back_queue = msgget((key_t)14535, 0666 | IPC_CREAT);
	
	
	 if (main_queue  == -1) 		// -1 means the message queue is not created  
     {  
             printf("Error in creating queue\n");  
             exit(0);  
     }
        
    //Creating user array 
    long Users[10];
	int size=1;			//start from 1, 0 has garbage (dunno why)
	
	while(1)
	{
		//printf("Waiting\n");
		struct message msg;
		msgrcv(main_queue, &msg, sizeof(msg), 0, 0);		//msgrcv=reading msg from queue
		
		if(strcmp(msg.buffer, "")==0)
		{
			Users[size]=msg.type;
			printf("New user added with pid: %d\n", Users[size]);
			size++;
		}
		else
		{
			printf("New msg recieved from: %d\n", msg.type);
			
			int exclude=msg.type;
			for(int i=1; i<size; i++)
			{
			    	if(Users[i]!=exclude)
			    	{
			    		msg.type=Users[i];
			    		//printf("Msg sent to: %d\n", Users[i]);
			    		int x=msgsnd(back_queue, &msg, sizeof(msg), 0);		//msgsnd=write a msg to msg queue
			    	}
			}        
		}

	}
	return 0;
}
