#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "fnd.h"

#define MODE_STATIC_DIS 's'
#define MODE_OFF 'o'

int main(int argc,char **argv)
{
	if(argc < 3)
	{
		printf("Your Command is Wrong \n");
		printf("[HELP] ./fndtest.out mode number \n");
	}
	char *mode = argv[1];
	int number = atoi(argv[2]);
	
	fndInit();
	
	switch(*mode)
	{
		case 's':
			fndDisp(number,0); break;
		case 'o':
			fndOff(); break;
		default: break;
	}
	
	fndExit();
}
