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

#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"

#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"

BUTTON_MSG_T TxButton;
pthread_t buttonTh_id;

int fd,msgID;
int readSize, inputIndex;
struct input_event stEvent;
char buttonPath[200] = {0, };

int probeButtonPath(char *newPath)
{
	int returnValue = 0;
	int number = 0;
	
	FILE *fp = fopen(PROBE_FILE,"rt");
	
	while(!feof(fp))
	{
		char tmpStr[200];
		fgets(tmpStr,200,fp);
		//printf("%s \n",tmpStr);
		if( strcmp(tmpStr,HAVE_TO_FIND_1) == 0)
		{
			printf("YES! I found: %s\r\n",tmpStr);
			returnValue = 1;
		}
		
		if( (returnValue == 1) && ( strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2) ) == 0 ) )
		{
			printf("-->%s",tmpStr);
			printf("\t%c\r\n", tmpStr[ strlen(tmpStr) -3 ] );
			number = tmpStr[strlen(tmpStr) -3 ] - '0';
			break;
		}
	}
	fclose(fp);
	if(returnValue == 1)
			sprintf (newPath, "%s%d",INPUT_DEVICE_LIST,number);
	return returnValue;
}

//while loop를 돌면서 read하다가 의미있는 값을 msgsnd
void* buttonThFunc()
{
	while(1)
	{
		TxButton.pressed = 0;
		readSize = read(fd, &stEvent, sizeof(stEvent));
		
		if(readSize != sizeof(stEvent) )
		{
			continue;
		}
		
		if( stEvent.type == EV_KEY )
		{
			printf("EV_KEY(");
			switch(stEvent.code)
			{
				case KEY_HOME: 			printf("Home key):"); 			TxButton.keyInput = 0; 	break;
				case KEY_BACK: 			printf("Back key):"); 			TxButton.keyInput = 1; 	break;
				case KEY_SEARCH: 		printf("Search Key):"); 		TxButton.keyInput = 2; 	break;
				case KEY_MENU: 			printf("Menu key):"); 			TxButton.keyInput = 3; 	break;
				case KEY_VOLUMEDOWN: 	printf("Volume down key):"); 	TxButton.keyInput = 4;	break;
				case KEY_VOLUMEUP:		printf("Volume up key):"); 		TxButton.keyInput = 5;	break;
				default: break;
			}
			
			if( stEvent.value ) 	{
				printf("pressed\n"); 
				TxButton.pressed = 1;
				TxButton.messageNum = 1;
				if( TxButton.pressed == 1 )
					msgsnd(msgID, &TxButton, sizeof(TxButton)-sizeof(TxButton.messageNum),0);
				
			}
			else
			{
					printf("released \n");
					//if( TxButton.pressed == 1 )
						//msgsnd(msgID, &TxButton, sizeof(TxButton)-sizeof(TxButton.messageNum),0);
						
			}
		}
		else ; // do Nothing
	}
}

int buttonInit()
{
	if (probeButtonPath(buttonPath) == 0)
	{
		printf("ERROR! File Not Found! \r\n");
		printf("Did you insmod?\r\n");
		return 0;
	}
	
	printf("buttonPath: %s\r\n", buttonPath);
	fd = open (buttonPath, O_RDONLY);
	
	msgID = msgget (MESSAGE_ID, IPC_CREAT|0666);
	pthread_create(&buttonTh_id, NULL, buttonThFunc, NULL);
	return 1;
}

int buttonExit()
{
	close(fd);
}
