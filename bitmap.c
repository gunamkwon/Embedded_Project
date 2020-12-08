#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include <sys/shm.h>
#include <sys/msg.h>
#include <pthread.h>
#include <unistd.h>

#include "button.h"
#include "libbitmap.h"

void* getData();
void* drawGraph();
void* Button_Thread();

static char data [1024*600*3] = {0,};
static int get_data=1;
static int *accel_data;
static int accel[3] = {0,};
static int first_loop = 1;
static int first_data = 1;

static int shmID;

static char *shmemAddr;

int msgID;
BUTTON_MSG_T RxButton;
    pthread_t thread_t2;
    pthread_t button;
int main()
{
    printf("open bitmap.out \r\n");
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int cols = 0, rows= 0;

 
	msgID = msgget(MESSAGE_ID,IPC_CREAT|0666);
 
    if(fb_init(&screen_width,&screen_height,&bits_per_pixel,&line_length) <0 )
    {
        printf("FrameBuffer Init Failed\r\n");
        return 0;
    }

    draw_background(data);
	pthread_create(&button,NULL,Button_Thread,NULL);
    pthread_create(&thread_t2,NULL,drawGraph,NULL);
    
    pthread_join(thread_t2,NULL);
    fb_clear();
    fb_close();
}


void* Button_Thread()
{
    printf("Button Thread Start\r\n");
    while(1)
    {
        int msgret = 0;
        msgret = msgrcv(msgID, &RxButton, sizeof(RxButton)-sizeof(RxButton.messageNum),0,0);
        if(msgret == -1)    break;
    }
    printf("There is no thrash\r\n");
    while(1)
    {
        int msgret = 0;
        msgret = msgrcv(msgID, &RxButton, sizeof(RxButton)-sizeof(RxButton.messageNum),0,0);
        printf("msgret = %d \r\n",msgret);
        if(msgret !=-1){
			printf("mode: %d \r\n",RxButton.keyInput);
		}
    }
} 

void* drawGraph()
{
    int x_temp[1021] = {0,}; 
    int y_temp[1021] = {0,};
    int z_temp[1021] = {0,};
    int cnt;
    static int accel_ex1;
    static int accel_ex2;
    static int accel_ex3;
    static int temp_ex1;
    static int temp_ex2;
    static int temp_ex3;
    int getdata_x=0,getdata_y=0,getdata_z=0;
	
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
            int x=0,y=0,z=0;
            for(cnt=3;cnt<=1024;)
            {   
               
                if(get_data==1)
                {
					accel_data = (int *)shmemAddr;
					//printf("accel_data: %d %d %d\r\n",accel_data[1],accel_data[2],accel_data[3]);
					
					if( ( (accel[0] = 300 - accel_data[0]/10) < 0) || ( (accel[0] = 300 - accel_data[0]/10) > 600) )
					{
						if(accel[0] < 300)  accel[0]= 0;
						else accel[0] = 600;
					}
					if( ( (accel[1] = 300 - accel_data[1]/10) < 0) || ( (accel[1] = 300 - accel_data[1]/10) > 600) )
					{	     
						if(accel[1] < 300)  accel[1]= 0;
						else accel[1] = 600;
					}	                
					if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
					{
						if(accel[2] < 300)  accel[2]= 0;
						else accel[2] = 600;
					}
					
					if(first_data==1){
						accel_ex1 = accel[0];
						accel_ex2 = accel[1];
						accel_ex3 = accel[2];
						first_data=0;
					}
					else{
						accel_ex1 = temp_ex1;
						accel_ex2 = temp_ex2;
						accel_ex3 = temp_ex3;
					}
                
					//printf("\t accel_ex: %d %d %d \r\n",accel_ex1,accel_ex2,accel_ex3);
					
					usleep(10);
					
					accel_data = (int *)shmemAddr;
					//printf("accel_data: %d %d %d\r\n",accel_data[0],accel_data[1],accel_data[2]);
					if(	RxButton.keyInput==5 )	pthread_cancel(thread_t2);
					if( ( (accel[0] = 300 - accel_data[0]/10) < 0) || ( (accel[0] = 300 - accel_data[0]/10) > 600) )
					{
						if(accel[0] < 300)  accel[0]= 0;
						else accel[0] = 600;
					}
					if( ( (accel[1] = 300 - accel_data[1]/10) < 0) || ( (accel[1] = 300 - accel_data[1]/10) > 600) )
					{	     
						if(accel[1] < 300)  accel[1]= 0;
						else accel[1] = 600;
					}	                
					if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
					{
						if(accel[2] < 300)  accel[2]= 0;
						else accel[2] = 600;
					}
					temp_ex1 = accel[0];
					temp_ex2 = accel[1];
					temp_ex3 = accel[2];
					//printf("\t accel: %d %d %d\r\n",accel[0],accel[1],accel[2]);
					get_data=0;
				}
                else
                {
                //printf("draw accel: %d %d %d \r\n",accel[0],accel[1],accel[2]);
                    // draw x line
                        data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+0] = 0xff; 
                        data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+1] = 0x00;
                        data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+2] = 0x00;
                        x_temp[cnt-3] = ((WIDTH*(accel_ex1+x))+WIDTH-cnt)*3;
                    // draw y line
                        data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+0] = 0x00;
                        data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+1] = 0x00;
                        data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+2] = 0xff;
                        y_temp[cnt-3] = ((WIDTH*(accel_ex2+y))+WIDTH-cnt)*3;
                    // draw z line
                        data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+0] = 0x00;
                        data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+1] = 0xff;
                        data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+2] = 0x00;
                        z_temp[cnt-3] = ((WIDTH*(accel_ex3+z))+WIDTH-cnt)*3;

                    if((accel[0]+x)>accel_ex1 ) x-=1;    else if((accel[0]+x)<accel_ex1)  x+=1; else    getdata_x=1;
                    if((accel[1]+y)>accel_ex2 ) y-=1;    else if((accel[1]+y)<accel_ex2)  y+=1; else    getdata_y=1;
                    if((accel[2]+z)>accel_ex3 ) z-=1;    else if((accel[2]+z)<accel_ex3) z+=1; else     getdata_z=1; 
                    fb_write(data);
                    //usleep(1);
                    if(getdata_x==1&&getdata_y==1&&getdata_z==1)
                    {   get_data=1; getdata_x=0; getdata_y=0; getdata_z=0;  x=0;y=0;z=0; cnt++;}
                }
            }
            
            first_loop=0;
        }

        else
        {
            int x=0,y=0,z=0;
            //printf("second loop!!!!!!!!!!!!!!!\r\n");
            for(cnt=3;cnt<=1024;cnt++)
            {
                if(get_data==1)
                {
					accel_data = (int *)shmemAddr;
					//printf("accel_data: %d %d %d\r\n",accel_data[1],accel_data[2],accel_data[3]);
					
					if( ( (accel[0] = 300 - accel_data[0]/10) < 0) || ( (accel[0] = 300 - accel_data[0]/10) > 600) )
					{
						if(accel[0] < 300)  accel[0]= 0;
						else accel[0] = 600;
					}
					if( ( (accel[1] = 300 - accel_data[1]/10) < 0) || ( (accel[1] = 300 - accel_data[1]/10) > 600) )
					{	     
						if(accel[1] < 300)  accel[1]= 0;
						else accel[1] = 600;
					}	                
					if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
					{
						if(accel[2] < 300)  accel[2]= 0;
						else accel[2] = 600;
					}
					
					if(first_data==1){
						accel_ex1 = accel[0];
						accel_ex2 = accel[1];
						accel_ex3 = accel[2];
						first_data=0;
					}
					else{
						accel_ex1 = temp_ex1;
						accel_ex2 = temp_ex2;
						accel_ex3 = temp_ex3;
					}
                
					//printf("\t accel_ex: %d %d %d \r\n",accel_ex1,accel_ex2,accel_ex3);
					
					usleep(10);
					
					accel_data = (int *)shmemAddr;
					//printf("accel_data: %d %d %d\r\n",accel_data[0],accel_data[1],accel_data[2]);
					if(	RxButton.keyInput==5 )	pthread_cancel(thread_t2);
					if( ( (accel[0] = 300 - accel_data[0]/10) < 0) || ( (accel[0] = 300 - accel_data[0]/10) > 600) )
					{
						if(accel[0] < 300)  accel[0]= 0;
						else accel[0] = 600;
					}
					if( ( (accel[1] = 300 - accel_data[1]/10) < 0) || ( (accel[1] = 300 - accel_data[1]/10) > 600) )
					{	     
						if(accel[1] < 300)  accel[1]= 0;
						else accel[1] = 600;
					}	                
					if( ( (accel[2] = 300 - accel_data[2]/10) < 0) || ( (accel[2] = 300 - accel_data[2]/10) > 600) )
					{
						if(accel[2] < 300)  accel[2]= 0;
						else accel[2] = 600;
					}
					temp_ex1 = accel[0];
					temp_ex2 = accel[1];
					temp_ex3 = accel[2];
					//printf("\t accel: %d %d %d\r\n",accel[0],accel[1],accel[2]);
					get_data=0;
				}
				else
				{
            // deque x line
                data[ x_temp[cnt-3]+0 ]= 0x00;
                data[ x_temp[cnt-3]+1 ]= 0x00;
                data[ x_temp[cnt-3]+2 ]= 0x00;
             // draw x line
			    data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+0] = 0xff; 
                data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+1] = 0x00;
                data[( (WIDTH*(accel_ex1+x))+WIDTH-cnt)*3+2] = 0x00;
                 x_temp[cnt-3] = ((WIDTH*(accel_ex1+x))+WIDTH-cnt)*3;
            //deque y line
                data[ y_temp[cnt-3]+0 ]= 0x00;
                data[ y_temp[cnt-3]+1 ]= 0x00;
                data[ y_temp[cnt-3]+2 ]= 0x00;
            // draw y line
                 data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+0] = 0x00;
                 data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+1] = 0x00;
                 data[( (WIDTH*(accel_ex2+y))+WIDTH-cnt)*3+2] = 0xff;
                  y_temp[cnt-3] = ((WIDTH*(accel_ex2+y))+WIDTH-cnt)*3;
            //deque z line
                data[ z_temp[cnt-3]+0 ]= 0x00;
                data[ z_temp[cnt-3]+1 ]= 0x00;
                data[ z_temp[cnt-3]+2 ]= 0x00;        
            // draw z line
                 data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+0] = 0x00;
                 data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+1] = 0xff;
                 data[( (WIDTH*(accel_ex3+z))+WIDTH-cnt)*3+2] = 0x00;
                 z_temp[cnt-3] = ((WIDTH*(accel_ex3+z))+WIDTH-cnt)*3;

                 if((accel[0]+x)>accel_ex1 ) x-=1;    else if((accel[0]+x)<accel_ex1)  x+=1; else    getdata_x=1;
                 if((accel[1]+y)>accel_ex2 ) y-=1;    else if((accel[1]+y)<accel_ex2)  y+=1; else    getdata_y=1;
                 if((accel[2]+z)>accel_ex3 ) z-=1;    else if((accel[2]+z)<accel_ex3) z+=1; else     getdata_z=1; 
				fb_write(data);
				//usleep(1);
                if(getdata_x==1&&getdata_y==1&&getdata_z==1)
                 {   get_data=1; getdata_x=0; getdata_y=0; getdata_z=0;  x=0;y=0;z=0; cnt++;}
				}
			}
        } 
    }      
}
