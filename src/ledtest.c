#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "led.h"

int main(int argc, char **argv)
{
	unsigned int data = 0;

	if(argc < 2)
	{
		perror("Args number is less than 2\n");
		return 1;
	}
	data = strtol(argv[1],NULL,10);
	printf("magnitude: %d",data);
	ledLibInit();
	
	ledsOn(data,1);
	ledLibExit();
}
