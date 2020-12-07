#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>

#include "libbitmap.h"

void* getData();
void* drawGraph();
pthread_mutex_t lock_graph;

static char data [1024*600*3] = {0,};
static int get_data=1;
static int *accel_data;
static int accel[3] = {0,};


static int fist_data = 1;

static int shmID;

static char *shmemAddr;

int main()
{
    printf("open bitmap.out \r\n");
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int cols = 0, rows= 0;
    pthread_t thread_t1,thread_t2;



    
    if(fb_init(&screen_width,&screen_height,&bits_per_pixel,&line_length) <0 )
    {
        printf("FrameBuffer Init Failed\r\n");
        return 0;
    }

    draw_background(data);

    pthread_create(&thread_t2,NULL,drawGraph,NULL);
    pthread_join(thread_t2,NULL);
    
    fb_close();
}

static int first_loop = 1;

void* drawGraph()
{
    int x_temp[1021] = {0,}; 
    int y_temp[1021] = {0,};
    int z_temp[1021] = {0,};
    int cnt;

    shmID = shmget((key_t)1234,1024,IPC_CREAT|0666);
    if(shmID == -1)
    {
		printf("Shmget RCV error! \r\n");
		exit(1);
	}
	   
	shmemAddr = ( (char *)shmat(shmID,NULL,0) );
	if( ((int *)shmemAddr) < 0)
	{
		printf("Shmat Error!\r\n");
		exit(1);
	}

    while(1)
    {   
        if(first_loop==1)
        {    
            //int x=0,y=0,z=0;
            for(cnt=3;cnt<=1024;cnt++)
            {   
                accel_data = (int *)shmemAddr;
                printf("accel_data: %d %d %d\r\n",accel_data[1],accel_data[2],accel_data[3]);
                if( ( (accel[0] = 300 - accel_data[0]/5) < 0) || ( (accel[0] = 300 - accel_data[0]/5) > 600) )
                {
                    if(accel[0] < 300)  accel[0]= 0;
                    else accel[0] = 600;
                }
                if( ( (accel[1] = 300 - accel_data[1]/5) < 0) || ( (accel[1] = 300 - accel_data[1]/5) > 600) )
                {     
                    if(accel[1] < 300)  accel[1]= 0;
                    else accel[1] = 600;
                }                
                if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
                {
                    if(accel[2] < 300)  accel[2]= 0;
                    else accel[2] = 600;
                }
                    printf("draw accel: %d %d %d \r\n",accel[0],accel[1],accel[2]);
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
                        data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+0] = 0x00;
                        data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+1] = 0xff;
                        data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+2] = 0x00;
                        z_temp[cnt-3] = ((WIDTH*(accel[2]))+WIDTH-cnt)*3;

                    //if(accel[0]>accel_ex1 ) x++;    else if(accel[0]<accel_ex1)  x--; else;
                    //if(accel[1]>accel_ex2 ) y++;    else if(accel[1]<accel_ex2)  y--; else;
                    //if(accel[2]>accel_ex3 ) z++;    else if(accel[2]<accel_ex3) z--; else;
                        
                    fb_write(data);
                    usleep(100000);
                }
            first_loop=0;
        }

        else
        {
            //int x=0,y=0,z=0;
            for(cnt=3;cnt<=1024;cnt++)
            {
                accel_data = (int *)shmemAddr;
               printf("accel_data: %d %d %d\r\n",accel_data[1],accel_data[2],accel_data[3]);
                if( ( (accel[0] = 300 - accel_data[0]/5) < 0) || ( (accel[0] = 300 - accel_data[0]/5) > 600) )
                {
                    if(accel[0] < 300)  accel[0]= 0;
                    else accel[0] = 600;
                }
                if( ( (accel[1] = 300 - accel_data[1]/5) < 0) || ( (accel[1] = 300 - accel_data[1]/5) > 600) )
                {     
                    if(accel[1] < 300)  accel[1]= 0;
                    else accel[1] = 600;
                }                
                if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
                {
                    if(accel[2] < 300)  accel[2]= 0;
                    else accel[2] = 600;
                }
            // deque x line
                data[ x_temp[cnt-3]+0 ]= 0x00;
                data[ x_temp[cnt-3]+1 ]= 0x00;
                data[ x_temp[cnt-3]+2 ]= 0x00;
             // draw x line
                data[( (WIDTH*(accel[0]))+WIDTH-cnt)*3+0] = 0xff; 
                data[( (WIDTH*(accel[0]))+WIDTH-cnt)*3+1] = 0x00;
                data[( (WIDTH*(accel[0]))+WIDTH-cnt)*3+2] = 0x00;
                x_temp[cnt-3] = ((WIDTH*(accel[0]))+WIDTH-cnt)*3;
            //deque y line
                data[ y_temp[cnt-3]+0 ]= 0x00;
                data[ y_temp[cnt-3]+1 ]= 0x00;
                data[ y_temp[cnt-3]+2 ]= 0x00;
            // draw y line
                data[( (WIDTH*(accel[1]))+WIDTH-cnt)*3+0] = 0x00;
                data[( (WIDTH*(accel[1]))+WIDTH-cnt)*3+1] = 0x00;
                data[( (WIDTH*(accel[1]))+WIDTH-cnt)*3+2] = 0xff;
                y_temp[cnt-3] = ((WIDTH*(accel[1]))+WIDTH-cnt)*3;
            //deque z line
                data[ z_temp[cnt-3]+0 ]= 0x00;
                data[ z_temp[cnt-3]+1 ]= 0x00;
                data[ z_temp[cnt-3]+2 ]= 0x00;        
            // draw z line
                data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+0] = 0x00;
                data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+1] = 0xff;
                data[( (WIDTH*(accel[2]))+WIDTH-cnt)*3+2] = 0x00;
                z_temp[cnt-3] = ((WIDTH*(accel[2]))+WIDTH-cnt)*3;
                        
                //if(accel[0]>accel_ex1 ) x++;    else  x--;
                //if(accel[1]>accel_ex2 ) y++;    else  y--;
                //if(accel[2]>accel_ex3 ) z++;    else  z--;
            }
        } 
    }      
}
