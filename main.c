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

int status = 0;

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

            ledLibInit();


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



}

