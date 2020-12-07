#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include <sys/shm.h>

#include "libbitmap.h"

#define FBDEV_FILE "/dev/fb0"

static int fbfd;
static int fbHeight=0;	//현재 하드웨어의 사이즈
static int fbWidth=0;	//현재 하드웨어의 사이즈
static unsigned long   *pfbmap;	//프레임 버퍼
static struct fb_var_screeninfo fbInfo;	//To use to do double buffering.
static struct fb_fix_screeninfo fbFixInfo;	//To use to do double buffering.


#define PFBSIZE 			(fbHeight*fbWidth*sizeof(unsigned long)*2)	//Double Buffering
#define DOUBLE_BUFF_START	(fbHeight*fbWidth)	///Double Swaping
static int currentEmptyBufferPos = 0;
//1 Pixel 4Byte Framebuffer.


int fb_init(int * screen_width, int * screen_height, int * bits_per_pixel, int * line_length)
{
    struct  fb_fix_screeninfo fbfix;

	if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE);
        return -1;
    }

    if( ioctl(fbfd, FBIOGET_VSCREENINFO, &fbInfo) )
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
		close(fbfd);
        return -1;
    }
   	if( ioctl(fbfd, FBIOGET_FSCREENINFO, &fbFixInfo) )
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        close(fbfd);
        return -1;
    }
	//printf ("FBInfo.YOffset:%d\r\n",fbInfo.yoffset);
	fbInfo.yoffset = 0;
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &fbInfo);	//슉!
    if (fbInfo.bits_per_pixel != 32)
    {
        printf("bpp is not 32\n");
		close(fbfd);
        return -1;
    }	

    fbWidth = *screen_width    =   fbInfo.xres;
    fbHeight = *screen_height   =   fbInfo.yres;
    *bits_per_pixel  =   fbInfo.bits_per_pixel;
    *line_length     =   fbFixInfo.line_length;

	pfbmap  =   (unsigned long *)
        mmap(0, PFBSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
	
	if ((unsigned)pfbmap == (unsigned)-1)
    {
        printf("fbdev mmap failed\n");
        close(fbfd);
		return -1;
    }
	#ifdef ENABLED_DOUBLE_BUFFERING
		currentEmptyBufferPos = DOUBLE_BUFF_START;	//더블버퍼링 임시 주소로 할당
	#else
		currentEmptyBufferPos = 0;
	#endif
	//printf ("CurrentEmptyBuffPos:%d\r\n",currentEmptyBufferPos);
	return 1;
}

void fb_clear(void)
{
	int coor_y = 0;
	int coor_x = 0;
	// fb clear - black
    for(coor_y = 0; coor_y < fbHeight; coor_y++) 
	{
        unsigned long *ptr =   pfbmap + currentEmptyBufferPos + (fbWidth * coor_y);
        for(coor_x = 0; coor_x < fbWidth; coor_x++)
        {
            *ptr++  =   0x000000;
        }
    }
	#ifdef ENABLED_DOUBLE_BUFFERING
		fb_doubleBufSwap();
	#endif
}

void fb_doubleBufSwap(void)
{
	if (currentEmptyBufferPos == 0)
	{
		fbInfo.yoffset = 0;
		currentEmptyBufferPos = DOUBLE_BUFF_START;
	}
	else
	{
		fbInfo.yoffset = fbHeight;
		currentEmptyBufferPos = 0;		
	}
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &fbInfo);	//슉!
}

void fb_close(void)
{
	printf ("Memory UnMapped!\r\n");
    munmap( pfbmap, PFBSIZE);
	printf ("CloseFB\r\n");
    close( fbfd);
}

void fb_write(char* picData)
{
	int coor_y=0;
	int coor_x=0;
	int targetHeight = fbHeight;	//if Screen과 파일 사이즈가 안맞으면
	int targetWidth = fbWidth;		//if Screen과 파일 사이즈가 안맞으면
	
	for(coor_y = 0; coor_y < targetHeight; coor_y++) 
	{
		int bmpYOffset = coor_y*fbWidth*3; ///Every 1Pixel requires 3Bytes.
		int bmpXOffset = 0;
		for (coor_x=0; coor_x < targetWidth; coor_x++)
		{
			//BMP: B-G-R로 인코딩 됨, FB: 0-R-G-B로 인코딩 됨.
			pfbmap[coor_y*fbWidth+ (fbWidth-coor_x) + currentEmptyBufferPos] = 
				((unsigned long)(picData[bmpYOffset+bmpXOffset+2])<<16) 	+
				((unsigned long)(picData[bmpYOffset+bmpXOffset+1])<<8) 		+
				((unsigned long)(picData[bmpYOffset+bmpXOffset+0]));
			bmpXOffset+=3;	//Three Byte.
		}
    }	
	#ifdef ENABLED_DOUBLE_BUFFERING
		fb_doubleBufSwap();
	#endif
}

void drawgraph(char *data)
{
    int shmID;
    shmID = shmget((key_t)1234,1024,IPC_CREAT|0666);
    if(shmID == -1)
    {
		printf("Shmget RCV error! \r\n");
		exit(1);
	}
	   
	char *shmemAddr = ( (char *)shmat(shmID,NULL,0) );
	if( ((int *)shmemAddr) < 0)
	{
		printf("Shmat Error!\r\n");
		exit(1);
	}
	   
	int *accel = (int *)shmemAddr;
    
    int x_temp[1021] = {0,}; 
    int y_temp[1021] = {0,};
    int z_temp[1021] = {0,};
    int cnt;

    for(cnt=3;cnt<=1024;cnt++)
    {
    // draw x line
        data[( (WIDTH*accel[0])+WIDTH-cnt)*3+0] = 0xff; 
        data[( (WIDTH*accel[0])+WIDTH-cnt)*3+1] = 0x00;
        data[( (WIDTH*accel[0])+WIDTH-cnt)*3+2] = 0x00;
        x_temp[cnt-3] = ((WIDTH*accel[0])+WIDTH-cnt)*3;
    // draw y line
        data[( (WIDTH*accel[1])+WIDTH-cnt)*3+0] = 0x00;
        data[( (WIDTH*accel[1])+WIDTH-cnt)*3+1] = 0x00;
        data[( (WIDTH*accel[1])+WIDTH-cnt)*3+2] = 0xff;
        y_temp[cnt-3] = ((WIDTH*accel[1])+WIDTH-cnt)*3;        
    // draw z line
        data[( (WIDTH*accel[2])+WIDTH-cnt)*3+0] = 0x00;
        data[( (WIDTH*accel[2])+WIDTH-cnt)*3+1] = 0xff;
        data[( (WIDTH*accel[2])+WIDTH-cnt)*3+2] = 0x00;
        z_temp[cnt-3] = ((WIDTH*accel[2])+WIDTH-cnt)*3;

        fb_write(data);
    }

    while(1)
    {
        printf("deque loop\r\n");
        cnt = 3;
        for(cnt=3;cnt<=1024;cnt++)
        {
        // deque x line
            data[ x_temp[cnt-3]+0 ]= 0x00;
            data[ x_temp[cnt-3]+1 ]= 0x00;
            data[ x_temp[cnt-3]+2 ]= 0x00;
        // draw x line
            data[( (WIDTH*accel[0])+WIDTH-cnt)*3+0] = 0xff; 
            data[( (WIDTH*accel[0])+WIDTH-cnt)*3+1] = 0x00;
            data[( (WIDTH*accel[0])+WIDTH-cnt)*3+2] = 0x00;
            x_temp[cnt-3] = ((WIDTH*accel[0])+WIDTH-cnt)*3;
        //deque y line
            data[ y_temp[cnt-3]+0 ]= 0x00;
            data[ y_temp[cnt-3]+1 ]= 0x00;
            data[ y_temp[cnt-3]+2 ]= 0x00;
        // draw y line
            data[( (WIDTH*accel[1])+WIDTH-cnt)*3+0] = 0x00;
            data[( (WIDTH*accel[1])+WIDTH-cnt)*3+1] = 0x00;
            data[( (WIDTH*accel[1])+WIDTH-cnt)*3+2] = 0xff;
            y_temp[cnt-3] = ((WIDTH*accel[1])+WIDTH-cnt)*3;
        //deque z line
            data[ z_temp[cnt-3]+0 ]= 0x00;
            data[ z_temp[cnt-3]+1 ]= 0x00;
            data[ z_temp[cnt-3]+2 ]= 0x00;        
        // draw z line
            data[( (WIDTH*accel[2])+WIDTH-cnt)*3+0] = 0x00;
            data[( (WIDTH*accel[2])+WIDTH-cnt)*3+1] = 0xff;
            data[( (WIDTH*accel[2])+WIDTH-cnt)*3+2] = 0x00;
            z_temp[cnt-3] = ((WIDTH*accel[2])+WIDTH-cnt)*3;

            fb_write(data);
        }

    }

}

void draw_background(char *data)
{
    // draw row
    for(int i=0;i<HEIGHT;i++)
    {
        data[((WIDTH*i)+WIDTH-3)*3+0] = 0xff;
        data[((WIDTH*i)+WIDTH-3)*3+1] = 0xff;
        data[((WIDTH*i)+WIDTH-3)*3+2] = 0xff;
    }

    // draw line
    for(int i=3;i<WIDTH;i++)
    {
        data[((WIDTH*300)+i)*3+0] = 0xff;
        data[((WIDTH*300)+i)*3+1] = 0xff;
        data[((WIDTH*300)+i)*3+2] = 0xff;
    }

    // draw GREEN Square + INFO
    for(int i=0;i<20;i++)
    {
        for(int j=0;j<10;j++)
        {
            data[((WIDTH*(590-j))+WIDTH-900+i)*3+0] = 0x00;    
            data[((WIDTH*(590-j))+WIDTH-900+i)*3+1] = 0xff;
            data[((WIDTH*(590-j))+WIDTH-900+i)*3+2] = 0x00;
        }
    }

    for(int i=0;i<10;i++)
    {
        data[((WIDTH*(585))+WIDTH-900-20-i)*3+0] = 0xff;    
        data[((WIDTH*(585))+WIDTH-900-20-i)*3+1] = 0xff;
        data[((WIDTH*(585))+WIDTH-900-20-i)*3+2] = 0xff;
    }
    // draw RED Square + INFO
    for(int i=0;i<20;i++)
    {
        for(int j=0;j<10;j++)
        {
            data[((WIDTH*(560-j))+WIDTH-900+i)*3+0] = 0x00;    
            data[((WIDTH*(560-j))+WIDTH-900+i)*3+1] = 0x00;
            data[((WIDTH*(560-j))+WIDTH-900+i)*3+2] = 0xff;
        }
    }

    for(int i=0;i<10;i++)
    {
        data[((WIDTH*(555))+WIDTH-900-20-i)*3+0] = 0xff;    
        data[((WIDTH*(555))+WIDTH-900-20-i)*3+1] = 0xff;
        data[((WIDTH*(555))+WIDTH-900-20-i)*3+2] = 0xff;
    }
    // draw GREEN Square + INFO
    for(int i=0;i<20;i++)
    {
        for(int j=0;j<10;j++)
        {
            data[((WIDTH*(530-j))+WIDTH-900+i)*3+0] = 0xff;    
            data[((WIDTH*(530-j))+WIDTH-900+i)*3+1] = 0x00;
            data[((WIDTH*(530-j))+WIDTH-900+i)*3+2] = 0x00;
        }
    }

    for(int i=0;i<10;i++)
    {
        data[( (WIDTH*(525))+WIDTH-900-20-i )*3+0] = 0xff;    
        data[( (WIDTH*(525))+WIDTH-900-20-i )*3+1] = 0xff;
        data[( (WIDTH*(525))+WIDTH-900-20-i )*3+2] = 0xff;
    }
    
    fb_write(data);
}