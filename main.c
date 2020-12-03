#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/shm.h"
#include "sys/types.h"
#include "pthread.h"

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

int status = 0;
int button_mode = 0;

pthread_t mode[3];
pthread_mutex_t lock;



int main()
{
    pid_t pid;
    pid = fork();

    if(pid > 0) // parent process
    {
        while( !(waitpid(pid,&status,WNOHANG)) )
        {
 
            // Shared Memory
            int shmID = shmget((key_t)1234,1024,IPC_CREAT|0666);
            if(shmID == -1)
            {
                printf("Shmget Error!\r\n");
            }

            char *shmemAddr;
            shmemAddr = shmat(shmID, (void *)NULL,0);
            if((int)(shmemAddr) == -1)
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

        }
    }

    else if(pid == 0) // child process
    {
        execl("","",(char *)0); // Excuting Graph
        if() // button push
            exit(0);
        
    }

    else
        printf("Fail to Fork\r\n");

    library_exit();
}

void library_init()
{
    ledLibInit();
    
    buttonInit();
    
    buzzerInit();
    
    fndInit();
    
    textlcdInit();
    temp_init();    
    
    pwmLedInit();
}

void library_exit()
{
    ledsOn(0,0);
    ledLibExit();

    buttonExit();

    buzzerExit();

    fndOff();
    fndExit();

    textlcdOff();
    temp_off();

    pwmInactiveAll();    
}

void* MagnitudeSensor()
{
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(button_mode == 0)
        {}

        else
        {
            pthread_mutex_unlock(&lock);
            usleep(1);
        }
    }

}

void* TempSensor()
{   
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(button_mode == 1)
        {}
        else
        {
                pthread_mutex_unlock(&lock);
                usleep(1);
        }
    }
}


void* LevelSensor()
{
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(button_mode == 2)
        {}

        else
        {
            pthread_mutex_unlock(&lock);
            usleep(1);
        }
    }

}