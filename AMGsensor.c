#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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
	printf("I read Accel %d, %d, %d $d\r\n",accel[0],accel[1],accel[2]);
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
	printf("I read Accel %d, %d, %d $d\r\n",accel[0],accel[1],accel[2]);
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
