#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "led.h"

static unsigned int ledValue = 0;
static int fd = 0;

int ledOnOff(int ledNum, int onoff)
{
	int i =1;
	i = i << ledNum;
	ledValue = ledValue & (~i);
	if(onoff != 0) ledValue |= i;
	write(fd, &ledValue,4);
}

int ledsOn(int ledCount, int onoff)
{
	int i= 0;
	if(ledCount >8) {
		printf("ledCount error! \n");
		return -1;
	}
	
	for(int j=0;j<ledCount;j++)
	{
		i += 1 << j;
	}
	ledValue = ledValue & (~i);
	if(onoff != 0) ledValue |= i;
	write(fd, &ledValue,4);
}

int ledLibInit(void)
{
	fd = open("/dev/periled",O_WRONLY);
	ledValue = 0;
}

int ledLibExit(void)
{
	ledValue = 0;
	// ledOnOff(0,0); // Only in Project
	close(fd);
}

