#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "textlcd.h"

int main(int argc,char **argv)
{
	if(argc < 3)
	{
		perror("Args number is less than 2\n");
		doHelp();
		return 1;
	}
	textlcdInit();
	
	int linenum = strtol(argv[1],NULL,10);
	printf("linenum: %d\n",linenum);
	textlcdwrite(argv[2],argv[3],linenum);
	textlcdOff();
}
