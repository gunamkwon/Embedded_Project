#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <pthread.h>
#include <math.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "AMGsensor.h"
#include "button.h"
#include "buzzer.h"
#include "colorled.h"
#include "fnd.h"
#include "temp.h"
#include "textlcd.h"
#include "led.h"


void library_init();
void library_exit();
void* TempSensor();
void* MagnitudeSensor();
void* LevelSensor();
void* Button_Thread();

int status = 0;
static int button_mode = 0;

BUTTON_MSG_T RxButton;
pthread_t mode[3];
pthread_t button;
pthread_mutex_t lock;

int msgID;
int shmID;
char *shmemAddr;


int main()
{
    msgID = msgget(MESSAGE_ID,IPC_CREAT|0666);
    pid_t pid;

    pid = fork();

   if(pid > 0) // parent process
   { 
        while( !(waitpid(pid,&status,WNOHANG)) ) 
        {
            // Shared Memory
            shmID = shmget((key_t)1234,1024,IPC_CREAT|0666);
            if(shmID == -1)
            {
                printf("Shmget Error!\r\n");
            }

            shmemAddr = shmat(shmID, (void *)NULL, 0);
            if( ((int)shmemAddr) == -1)
            {
                printf("Shmat Error!\r\n");
                return -2;
            }

            //thread_mutex_init
            if(pthread_mutex_init(&lock,NULL)   !=0 )
            {
                printf("\n Mutex Init Failed\r\n");
                return 1;
            }

            library_init();
            pthread_create(&button,NULL,&Button_Thread,NULL);
            pthread_create(&(mode[0]),NULL,&MagnitudeSensor,NULL);
            pthread_create(&(mode[1]),NULL,&TempSensor,NULL);
            pthread_create(&(mode[2]),NULL,&LevelSensor,NULL);
            

            pthread_join(mode[0],NULL);
            pthread_join(mode[1],NULL);
            pthread_join(mode[2],NULL);
            pthread_join(button,NULL);

        }
    }

    else if(pid == 0) // child process
    {
        execl("/home/ecube/PROJECT/bitmap.out","bitmap.out",(char *)0); // Excuting GraphWW
    }

    else
        printf("Fail to Fork\r\n");

    library_exit();
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

void* MagnitudeSensor()
{
    while(1)
    {
		int i=0;
        pthread_mutex_lock(&lock);
        if(button_mode == 0)
        { 
            textlcdwrite("Magnitude Sensor","Stage: Normal",0);
			for(int i=0;i<5;i++){
				int *trash = getAccelerometer_default();
			}
            int *accel_default = getAccelerometer_default();
            double accelavg_default = getAverage(accel_default);
            //printf("accelavg_default: %lf \r\n",accelavg_default);
            
            while(i!=1)
            {   
				int *accel_now = getAccelerometer();
                double accelavg_now = getAverage(accel_now);
                //printf("accelavg_now: %f \r\n",accelavg_now);
                int *data = (int *)shmemAddr;
                for(int i=0;i<3;i++){
                    data[i] = accel_now[i]/10;
                    //printf("sharedMemory %d: %d\r\n",i,data[i]);
                }
            
                int magnitude = setMagnitude(accelavg_default,accelavg_now);
                printf("magnitude: %d\r\n",magnitude);
                ledsOn(magnitude,1);
                
                if(magnitude >= 3 && magnitude <= 6) // Warning Stage: Yellow
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: Yellow","",2);
                //    buzzerYellow();  // execl();?
                    pwmSetYellow();
                } 
                else if(magnitude > 6) // Warning Stage: Red
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: RED   ","",2);
                //    buzzerRed();    
                    pwmSetRed();
                }
                else
                {
                    buzzerStopSound();
                    textlcdwrite("Stage: Normal","",2);
                    pwmSetGreen();
                }
                button_mode = RxButton.keyInput;
				printf("mode2: %d\r\n",button_mode);
                usleep(10000);
                if(button_mode != 0) i=1; 
                
            }
            pthread_mutex_unlock(&lock);
        }   
        else
        {
            ledsOn(0,0);
            buzzerStopSound();
            pthread_mutex_unlock(&lock);
            usleep(1);
        }
    }
}

void* TempSensor()
{   
    while(1)
    {	
        
		int i=0;
        pthread_mutex_lock(&lock);
        if(button_mode == 1)
        {   
            textlcdwrite("Temp Sensor     ","Stage: Normal",0);
			printf("TempThread  Start\r\n");
            while(i!=1)
            {
                double temp_now = getTemperature();
                printf("temperature_now: %lf \r\n",temp_now);
                fndDisp(temp_now,0);    // Display Temperature in FND
                if(temp_now > 31 && temp_now <= 33)    // Waring Stage: Yellow
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: Yellow","",2);
                //    buzzerYellow(); //execl();
                    pwmSetYellow();
                }
                else if( temp_now > 33) // Waring Stage: Red
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: RED   ","",2);
                //    buzzerRed();//execl();//   
                    pwmSetRed();
                }
                else
                {
                    buzzerStopSound();
                    textlcdwrite("Stage: Normal","",2);
                    pwmSetGreen();
                }
                button_mode = RxButton.keyInput;
                printf("mode2: %d\r\n",button_mode);
                usleep(10000);
                
                if(button_mode != 1) i=1;    
            }
            pthread_mutex_unlock(&lock);
        }
        else
        {
            buzzerStopSound();
            pthread_mutex_unlock(&lock);
            usleep(1);
        }
    }
}


void* LevelSensor()
{
    while(1)
    {
        int i=0;
        pthread_mutex_lock(&lock);
        if(button_mode == 2)
        {
            textlcdwrite("Level Sensor    ","Stage: Normal",0);
            int * level_default = getGyroscope();
            double levelavg_default = getAverage(level_default);
            while(i!=1)
            {
                int * level_now = getGyroscope();
                double levelavg_now = getAverage(level_now);
                if(abs(levelavg_default - levelavg_now) > 10 && abs(levelavg_default - levelavg_now) <= 20) // Waring Stage: Yellow
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: Yellow","",2);// NEED TO SHOW TEXTLCD (MODE)
                //    buzzerYellow(); //execl();//    
                    pwmSetYellow();
                }
                else if(abs(levelavg_default - levelavg_now) > 20) // Waring Stage: Red
                {
                    buzzerStopSound();
                    textlcdwrite("Warning: RED   ","",2);
                //    buzzerRed(); //execl();//    
                    pwmSetRed();
                }
                else
                {
                    buzzerStopSound();
                    textlcdwrite("Stage: Normal","",2);
                    pwmSetGreen();
                }
                button_mode = RxButton.keyInput;
                printf("mode2: %d\r\n",button_mode);                
                usleep(10000);
                if(button_mode != 2) break;                  
            }
            pthread_mutex_unlock(&lock);
        }

        else
        {
            buzzerStopSound();
            pthread_mutex_unlock(&lock);
            usleep(1);
        }
    }

}


////////////////////////////// Useless Function
void library_init()
{
    ledLibInit();

    buttonInit();

    buzzerInit();
    
    fndInit();

    textlcdInit();
    
    pwmLedInit();
    temp_init();    
    printf("INIT FINISHED \r\n");
}

void library_exit()
{
    ledsOn(0,0);
    buzzerStopSound();
    ledLibExit();

    buttonExit();

    buzzerExit();
    
    fndOff();
    fndExit();

    textlcdOff();
    temp_off();

    pwmInactiveAll();
    
    printf("EXIT FINISHED \r\n");
}