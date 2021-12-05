#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include "buzzer.h"
#include "AMGsensor.h"


int main()
{
	buzzerInit();
	
	int *f_accel = (int *)malloc(sizeof(int) * 3);
	f_accel = getAccelerometer_default();
	sleep(5);
	
	for(;;)
	{
		int *accel = getAccelerometer();
		sleep(1);
		printf("%d %d \n",f_accel[0],accel[0]);
		printf("no array:%d %d \n",*f_accel, *accel); 
		int acceldif = abs(f_accel[0]-accel[0]);
		printf("%d \n",acceldif);
		if(acceldif > 1000){
			buzzerPlaySound(5);
			sleep(3);
			buzzerStopSound();
		}
	}
	free(f_accel);
	return 0;
}
