#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/msg.h>
#include <pthread.h>

#include "button.h"
#include "led.h"

int main(int argc, char *argv[])
{
	BUTTON_MSG_T RxButton;
	int msgID = msgget(MESSAGE_ID,IPC_CREAT|0666);
	
	ledLibInit();
	buttonInit();
	
		while(1)
		{
			int msgret = 0; 
			msgret = msgrcv(msgID, &RxButton, sizeof(RxButton)-sizeof(RxButton.messageNum),0,0);	
			
			if(msgret != -1){
			printf("led %d ON \n",RxButton.keyInput);
			ledsOn(RxButton.keyInput,1);
			}
		}
		
	ledLibExit();
	buttonExit();
}
