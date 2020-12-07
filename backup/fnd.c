#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "fnd.h"


#define FND_DRIVER_NAME "/dev/perifnd"

#define MAX_FND_NUM 6
#define FND_DATA_BUFF_LEN (MAX_FND_NUM +2)

typedef struct FNDWriteDataForm_tag
{
	char DataNumeric[FND_DATA_BUFF_LEN];
	char DataDot[FND_DATA_BUFF_LEN];
	char DataValid[FND_DATA_BUFF_LEN];
}stFndWriteForm,*pStFndWriteForm;

static int fd;
stFndWriteForm stWriteData;

int fndInit()
{	
	fd = open(FND_DRIVER_NAME,O_RDWR);
	if(fd < 0)
	{
		perror("driver open error\n");
		return -1;
	}
}

int fndDisp(int num,int dotflag)
{
	
	int temp,i;
	stFndWriteForm stWriteData;
	for(i=0; i< MAX_FND_NUM; i++)
	{
		stWriteData.DataDot[i] = (dotflag & (0x1 << i)) ? 1:0;
		stWriteData.DataValid[i] = 1;
	}
	// if 6 fnd
	temp = num % 1000000; stWriteData.DataNumeric[0] = temp / 100000;
	temp = num % 100000; stWriteData.DataNumeric[1] = temp / 10000;
	temp = num % 10000; stWriteData.DataNumeric[2] = temp / 1000;
	temp = num % 1000; stWriteData.DataNumeric[3] = temp / 100;
	temp = num % 100; stWriteData.DataNumeric[4] = temp / 10;
	stWriteData.DataNumeric[5] = num % 10;
	
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	return 1;
}

int fndOff()
{
	for(int i=0; i< MAX_FND_NUM; i++)
		stWriteData.DataValid[i] = 0;
		
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	return 1;
}

int fndExit()
{
	close(fd);
}
