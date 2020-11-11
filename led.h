#ifndef _LED_H_
#define _LED_H_

int ledLibInit(void);
int ledOnOff(int ledNum,int onoff);
int ledsOn(int ledCount,int onoff);
int ledLibExit(void);

#define LED_DRIVER_NAME "/dev/periled"

#endif

