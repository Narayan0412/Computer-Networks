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

int main(){
	int n=3;	
	int msgqr = msgget((key_t)10001, 0666 | IPC_CREAT);
	int msgqw = msgget((key_t)10002, 0666 | IPC_CREAT);
	msgctl(msgqr, IPC_RMID, NULL);	
	msgctl(msgqw, IPC_RMID, NULL);
	msgqr = msgget((key_t)10001, 0666 | IPC_CREAT);				
	msgqw = msgget((key_t)10002, 0666 | IPC_CREAT);
	
	
	//char groups[4][10];
	printf("groups creating\n");
	//group 0 is dead
	//strcpy(groups[1],"123");
	//strcpy(groups[2],"12");
	//strcpy(groups[3],"13");
	//printf("groups created\n");
	
	int num[11];
	int size=1;
	char group[4][10];
	strcpy(group[1],"");
	strcpy(group[2],"");
	strcpy(group[3],"");
	printf("groups creating\n");
	
	 if (msgqr == -1 || msgqw == -1){  
                 printf("Error in creating queue\n");  
                 exit(0);  
         }
	

	while(1){
		struct message msg;
		msgrcv(msgqr, &msg, sizeof(msg), 0, 0);	
		if(msg.buffer[0]=='C'){
			int takenpid=0; 
			for(int i=1;msg.buffer[i]!=':';i++){
				takenpid=takenpid*10+(msg.buffer[i]-'0');
			}
			printf("takenpid is %d\n",takenpid);
			//char cnum=msg.buffer[1]; //change cname to index of pid
			char cnum;
			for(int i=1;i<size;i++){
				if(num[i]==takenpid){
					cnum=i+'0';
					break;
				}
			}
			printf("cnum is %d\n",takenpid);
			int grp=msg.type;
			int flag=0;
			for(int i=0;i<strlen(group[grp]);i++){
				if(group[grp][i] == cnum){
					flag=1;
					break;
				}
			}
			if(flag){				//if the person is in the group
				struct message msg2;
				strcpy(msg2.buffer,"group");
				char x[1];
				x[0] = (char)(grp+'0');
				strcat(msg2.buffer,x);
				strcat(msg2.buffer,": ");
				strcat(msg2.buffer,msg.buffer);
				printf("%s\n",msg2.buffer);
				for(int i=0; i<strlen(group[grp]); i++){
				    	if(group[grp][i] != cnum){
				    		//msg2.type=groups[grp][i]-'0'; //msg.type must be pid taken from index 
				    		msg2.type=num[group[grp][i]-'0'];
				    		int x=msgsnd(msgqw, &msg2, sizeof(msg2), 0);
				    	}
				}  
			}      
			else{
				struct message msg2;
				//msg2.type=cnum-'0'; //msg.type must be pid taken from index 
				msg2.type=takenpid;
				strcpy(msg2.buffer,"you are not part of this group");
				int x=msgsnd(msgqw, &msg2, sizeof(msg2), 0);
			}
		}
		else{
			num[size]=msg.type;
			char x[1];
			x[0] = (char)(size+'0');
			for(int i=0;i<strlen(msg.buffer);i++){
				strcat(group[msg.buffer[i]-'0'],x);
				printf("%d has %s\n",i,group[msg.buffer[i]-'0']);
			}
			printf("%d assigned %d\n",msg.type,size);
			size++;
		}
	}


	return 0;
}
