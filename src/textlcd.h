#ifndef _TEXTLCD_H_
#define _TEXTLCD_H_

#define LINE_NUM 		2
#define COLUMN_NUM 	16

int textlcdInit();
int textlcdwrite(const char *str1,const char *str2,int lineFlag);
int textlcdOff();

#endif
