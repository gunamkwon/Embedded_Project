#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "buzzer.h"
#define MAX_SCALE_STEP		8										// Scale 몇단계인지
#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices/"		// Buzzer driver 위치
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"

char gBuzzerBaseSysDir[128];  // /sys/bus/platform/devices/peribuzzer.XX

const int musicScale[MAX_SCALE_STEP]=				//Buzzer Scale 구조체 이 값에 따라 음 달라진다.
{
	262, /*do*/ 294,339,349,392,440,494, /*si*/ 523
};

DIR *dir_info;

int findBuzzerSysPath() {
	dir_info = opendir(BUZZER_BASE_SYS_PATH);
	int ifNotFound = 1;
	if(dir_info !=NULL)
	{
		while(1)
		{
			struct dirent *dir_entry;
			dir_entry = readdir (dir_info);
			if(dir_entry == NULL) break;
			if(strncasecmp(BUZZER_FILENAME,dir_entry->d_name,strlen(BUZZER_FILENAME)) == 0)
			{
				ifNotFound = 0;
				sprintf(gBuzzerBaseSysDir,"%s%s/",BUZZER_BASE_SYS_PATH,dir_entry->d_name);
			}
		}
	}

	printf("find %s\n" ,gBuzzerBaseSysDir);
	return ifNotFound;
}

void doHelp()
{
	printf("Usage: \n");
	printf("buzzertest <buzzerNo> \n");
	printf("buzzerNo: \n");
	printf("do(1),re(2),me(3),fa(4),sol(5),ra(6),si(7),do(8) \n");
	printf("off(0) \n");
}

void buzzerEnable(int bEnable)
{
	char path[200];
	sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_ENABLE_NAME);
	int fd = open(path,O_WRONLY);
	if(bEnable)	write(fd, &"1",1);
	else		write(fd, &"0",1);
	close(fd);
}

void setFrequency(int frequency)
{
	char path[200];
	sprintf(path,"%s%s",gBuzzerBaseSysDir,BUZZER_FREQUENCY_NAME);
	int fd = open(path,O_WRONLY);
	dprintf(fd,"%d",frequency);
	close(fd);
}

int buzzerPlaySound(int scale)
{
	if(scale > MAX_SCALE_STEP)
	{
		printf("<buzzerNO> over range \n");
		doHelp();
		return 1;
	}
	
	if(scale == 0)
		buzzerEnable(0);
	else
	{
		setFrequency(musicScale[scale -1]);
		buzzerEnable(1);
	}
}

void buzzerYellow()
{
	for(;;)
	{	
		setFrequency(musicScale[4]);
		buzzerEnable(1);
	}
}

void buzzerRed()
{
	for(;;)
	{
		setFrequency(musicScale[7]);
		buzzerEnable(1);
				
	}
}





int buzzerStopSound()
{
	buzzerEnable(0);
}





int buzzerInit()
{
	if(findBuzzerSysPath() == 1)
	{
		printf("Find Buzzer path Error!\n");
		printf("Do you insmod? \n");
		return -1;
	}
	
	printf("buzzer test \n");
	buzzerPlaySound(1);
	sleep(1);
	buzzerStopSound();
}

int buzzerExit()
{
	closedir(dir_info);
}