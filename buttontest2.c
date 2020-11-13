#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define INPUT_DEVICE_LIST

#define PROBE_FILE

int main(int argc, char *argv[])
{
int fp;
int readSize,inputIndex;
char inputDevPath[200]={0,};
if ( probeButtonPath(inputDevPath) == 0)
{
printf ("ERROR! File Not Found!\r\n");
printf ("Did you insmod?\r\n");
return 0;
}
printf ("inputDevPath: %s\r\n", inputDevPath);
fp = open(inputDevPath, O_RDONLY);

bttonInit();
BUTTON_MSG_T msgRx;
int msgID =msgget(MESSAGE_ID, IPC_CREAT|0666);

while(1)
{
int returnValue=0;
returnValue=msgcrv(msgID,&msgRx,sizeof(8),0,IPC_NOWAIT);
if(returnValue==1)
{
switch(msgRx.keyInput)
{
case KEY_VOLUMEUP: printf("Volume up key):"); break;
case KEY_HOME: printf("Home key):"); break;
case KEY_SEARCH: printf("Search key):"); break;
case KEY_BACK: printf("Back key):"); break;
case KEY_MENU: printf("Menu key):"); break;
case KEY_VOLUMEDOWN: printf("Volume down key):");
}
}
}
}
