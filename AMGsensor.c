#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define ACCELPATH "/sys/class/misc/FreescaleAccelerometer/"
#define MAGNEPATH "/sys/class/misc/FreescaleMagnetometer/"
#define GYROPATH "/sys/class/misc/FreescaleGyroscope/"

int fd =0;
FILE *fp = NULL;

int* getAccelerometer()
{
	fd = open(ACCELPATH "enable", O_WRONLY);
	dprintf(fd,"1");
	close(fd);

	fp = fopen(ACCELPATH "data", "rt");
    static	int accel[3];
	
	fscanf(fp,"%d, %d, %d",&accel[0],&accel[1],&accel[2]);
	printf("I read Accel %d, %d, %d\r\n",accel[0],accel[1],accel[2]);
	fclose(fp);
    return accel;
}

int* getAccelerometer_default()
{
	fd = open(ACCELPATH "enable", O_WRONLY);
	dprintf(fd,"1");
	close(fd);

	fp = fopen(ACCELPATH "data", "rt");
    static	int accel[3];
    static int accel_def[3];
	fscanf(fp,"%d, %d, %d",&accel[0],&accel[1],&accel[2]);
	printf("I read Accel %d, %d, %d\r\n",accel[0],accel[1],accel[2]);
	fclose(fp);
	for(int i=0;i<3;i++){
		accel_def[i] = accel[i];
	}
    return accel_def;
}

int* getMagnetometer()
{
	fd = open(MAGNEPATH "enable",O_WRONLY);
	dprintf(fd,"1");
	close(fd);

	fp = fopen(MAGNEPATH "data", "rt");
	
	static int magne[3];
	fscanf(fp,"%d, %d, %d",&magne[0],&magne[1],&magne[2]);
	printf("I read Magneto %d, %d, %d\r\n",magne[0],magne[1],magne[2]);
	fclose(fp);
    return magne;
}

int* getGyroscope()
{
	fd = open(GYROPATH "enable", O_WRONLY);
	dprintf(fd,"1");
	close(fd);

	fp = fopen(GYROPATH "data", "rt");
	static int gyro[3];

	fscanf(fp, "%d, %d, %d",&gyro[0],&gyro[1],&gyro[2]);
	printf("I read Gyroscope %d, %d, %d\r\n",gyro[0],gyro[1],gyro[2]);
	fclose(fp);
	return gyro;
}


double getAverage(int *data)
{ 
	double temp = data[1]*data[1]+ data[2]*data[2] + data[3]*data[3];
	return sqrt(temp); 
}

int setMagnitude(double avg_default,double avg_now)
{
	int magnitude;
	switch( abs(avg_default-avg_now) )
	{
		case 0 ... 10:
			magnitude = 1;
			break;		
		case 11 ... 20:
			magnitude = 2;
			break;		
		case 21 ... 30:
			magnitude = 3;
			break;		
		case 31 ... 40:
			magnitude = 4;
			break;		
		case 41 ... 50:
			magnitude = 5;
			break;		
		case 51 ... 60:
			magnitude = 6;
			break;		
		case 61 ... 70:
			magnitude = 7;
			break;		
		case 71 ... 80:
			magnitude = 8;
			break;
		case 81 ... 90:
			magnitude = 9;
			break;
		default:
			magnitude = 10;
			break;
	}
	return magnitude;
}