#ifndef AMGSENSOR_H_
#define AMGSENSOR_H_

int* getAccelerometer();
int* getAccelerometer_default();
int* getMagnetometer();
int* getGyroscope();

double getAverage(int* returnvalue);
int setMagnitude(double avg_default, double avg_now);

#endif
