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


#define INPUT_DEVICE_LIST "/dev/input/event5"

#define PROBE_FILE "/proc/bus/input/devices"

#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"


int fd;
int msgID=0;
pthread_t buttonTh_id;
char buttonPath;
void *buttonThFunc(void*a);


int probeButtonPath(char *newPath)
{
    int returnValue =0;
    int number =0;
    FILE *fp = fopen(PROBE_FILE,"rt");
    while(!feof(fp))
{
    char tmpStr[200];
    fgets(tmpStr,200,fp);
    if (strcmp(tmpStr,HAVE_TO_FIND_1) == 0)
{
    printf("YES! I found!: %s/r/n", tmpStr);
    returnValue = 1;
}
if (
      (returnValue == 1) &&
    (strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0)
)
{
    printf("-->%s", tmpStr);
    printf("/t%c/r/n",tmpStr[strlen(tmpStr)-3]);
    number = tmpStr[strlen(tmpStr)-3] - '0';
    break;
}
}
fclose(fp);
if (returnValue == 1)
sprintf (newpath,"%s%d",INPUT_DEVICE_LIST,number);
return returnValue;
}




int buttonInit(void)
{
	if(probeButtonPath(buttonPath) == 0)
		return 0;
	fd=open (buttonPath, O_RDONLY);
	msgID = msgget (MESSAGE_ID, IPC_CREAT|0666);
	prthread_create(&buttonTh_id, NULL, buttonThFunc, NULL);
	return 1;
}

int buttonExit(void)
{	
	phtread_cancel(&buttonTh_id);
	close(fd);
}


int probeButtonPath(char *newPath)
{
    int returnValue =0;
    int number =0;
    FILE *fp = fopen(PROBE_FILE,"rt");
#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"
    while(!feof(fp))
{
    char tmpStr[200];
    fgets(tmpStr,200,fp);
    if (strcmp(tmpStr,HAVE_TO_FIND_1) == 0)
{
    printf("YES! I found!: %s/r/n", tmpStr);
    returnValue = 1;
}
if (
      (returnValue == 1) &&
    (strncasecmp(tmpStr, HAVE_TO_FIND_2, strlen(HAVE_TO_FIND_2)) == 0)
)
{
    printf("-->%s", tmpStr);
    printf("/t%c/r/n",tmpStr[strlen(tmpStr)-3]);
    number = tmpStr[strlen(tmpStr)-3] - '0';
    break;
}
}
fclose(fp);
if (returnValue == 1)
sprintf (newpath,"%s%d",INPUT_DEVICE_LIST,number);
return returnValue;
}



void *buttonThFunc(void*a)
{
    
    BUTTON_MSG_T msgTx;
    msgTx.messageNum = 1;
    struct input_event stEvent;

   while(1)
{
    msgTx.keyInput=0;
   read(fd, &stEvent, sizeof(stEvent));
    
    if((stEvent.type==EV_KEY)&&(stEvent.value==0))
    {
        msgTx.keyInput =stEvent.code;
      msgTx.pressed=stEvent.value;
         msgsnd(msgID,&msgTx,sizeof(int),0);
    }
}
}



