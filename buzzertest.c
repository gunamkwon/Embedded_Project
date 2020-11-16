#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "buzzer.h"

int main(int argc,char **argv)
{	
	int freIndex;
	if(argc < 2 )
	{
		printf("Error!\n");
		doHelp();
		return 1;
	}
	
	buzzerInit();
	freIndex = atoi(argv[1]);
	
	buzzerPlaySound(freIndex);
	sleep(2);  // 2초동안 소리남
	
	buzzerStopSound();달
	sleep(1); // 2초 후 소리 Stop한것 (1초는 그냥 기)
	
	buzzerPlaySound(freIndex);
	buzzerStopSound(0);  // 이러면 소리 안남
	
	buzzerExit();
}
