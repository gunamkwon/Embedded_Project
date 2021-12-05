#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "textlcd.h"

#define TEXTLCD_DRIVER_NAME "/dev/peritextlcd"
#define LINE_BUFF_NUM (COLUMN_NUM + 4)
#define MODE_CMD 0
#define MODE_DATA 1

#define CMD_DISPLAY_MODE	0x10
	#define BIT_DISPLAY_MODE_CURSOR_DISP	0x01
	#define BIT_DISPLAY_MODE_CURSOR_BLINK	0x02
	#define BIT_DISPLAY_MODE_DISP_ENABLE	0x04
	
#define CMD_CURSOR_MOVE_MODE	0x11
	#define CURSOR_MOVE_MODE_ENABLE			0x01
	#define CURSOR_MOVE_MODE_RIGHT_DIR		0x02

#define CMD_CURSOR_MOVE_POSITION	 0x12
	#define CURSOR_MOVE_X_MASK				0x3F
	#define CURSOR_MOVE_Y_MASK				0xC0
	
#define CMD_WRITE_STRING	0x20
	#define CMD_DATA_WRITE_BOTH_LINE 	0
	#define CMD_DATA_WRITE_LINE_1		1
	#define CMD_DATA_WRITE_LINE_2		2

typedef struct TextLCD_tag
{
	unsigned char cmd;
	unsigned char cmdData;
	unsigned char reserved[2];
	
	char TextData[LINE_NUM][LINE_BUFF_NUM];
}stTextLCD,*pStTextLCD;

stTextLCD stlcd;
static int fd;

int textlcdInit()
{
	fd = open(TEXTLCD_DRIVER_NAME,O_RDWR);
	if(fd < 0)
	{
		perror("driver (//dev//peritextlcd) open error.\n");
		return 1;
	}	
}

int textlcdwrite(const char *str1,const char *str2,int lineFlag)
{	
	int len;
	memset(&stlcd,0,sizeof(stTextLCD));
	
	switch(lineFlag)
	{
		case 0:
			stlcd.cmdData = CMD_DATA_WRITE_BOTH_LINE; break;
		case 1:
			stlcd.cmdData = CMD_DATA_WRITE_LINE_1; break;
		case 2:
			stlcd.cmdData = CMD_DATA_WRITE_LINE_2; break;
		default:
			printf("lineFlag: %d wrong. range(1 ~ 2)\n",lineFlag);
			return 1; break;
	}
	
	if(lineFlag == 1 || lineFlag == 2)
	{
		len = strlen(str1);
		if(len > COLUMN_NUM)
			memcpy(stlcd.TextData[stlcd.cmdData - 1],str1,COLUMN_NUM);
		else
			memcpy(stlcd.TextData[stlcd.cmdData - 1],str1,len);
	}
	
	if(lineFlag == 0)
	{
		len = strlen(str1);
		if(len > COLUMN_NUM)
			memcpy(stlcd.TextData[0],str1,COLUMN_NUM);
		else
			memcpy(stlcd.TextData[0],str1,len);
			
		len = strlen(str2);
		if(len > COLUMN_NUM)
			memcpy(stlcd.TextData[1],str2,COLUMN_NUM);
		else
			memcpy(stlcd.TextData[1],str2,len);		
	}
		
	stlcd.cmd = CMD_WRITE_STRING;
	write(fd,&stlcd,sizeof(stTextLCD));
	return 1;
}

int textlcdOff()
{
	close(fd);
}