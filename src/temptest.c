#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "temp.h"

int main()
{
	temp_init();
	for(;;){
		getTemperature();
		sleep(1);
	}
	temp_off();
	
	return 0;
}
