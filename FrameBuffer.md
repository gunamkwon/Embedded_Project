## 1. 개요

프로젝트에서 실시간 그래프 화면을 구현하기 위해서는 LCD 화면을 수정해야했다.
LCD 화면을 수정하기 위해 프레임버퍼 디바이스 드라이버를 사용했다.

> **프레임 버퍼?**
> Frame Buffer는 커널에서 한 화면을 구성하기 위해 할당 해놓은 메모리.
> 그래픽 하드웨어를 사용자 레벨의 응용 프로그램이 제어할 수 있도록 만들어 놓은 것이
> **`프레임 버퍼 드라이버`** 이다.

## 2. 프레임 버퍼 데이터 불러오기

프레임버퍼 드라이버를 불러오기 위해서는 `"/dev/fb0"` 파일을 오픈해야한다.

```
	if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE);
        return -1;
    }
```

이후, `ioctl( )` 함수를 통해 필요한 커널 영역의 버퍼를 불러왔다.

```
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

    //yoffSet이 0이 아닐 경우, 처리하기 복잡해서 default값 수정
    //printf ("FBInfo.YOffset:%d\r\n",fbInfo.yoffset);
	fbInfo.yoffset = 0;
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &fbInfo);
    if (fbInfo.bits_per_pixel != 32)
    {
        printf("bpp is not 32\n");
		close(fbfd);
        return -1;
    }
```

불러온 프레임버퍼를 프로그램의 주소 공간 메모리에 대응시켜 수정을 해야하기 때문에
`mmap( )` 함수를 이용하여 `pfbmap` 이라는 변수에 대응시켜줬다.
앞으로 이 변수가 프레임버퍼를 수정할 것이다.

```
	pfbmap  =   (unsigned long *)
        mmap(0, PFBSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

	if ((unsigned)pfbmap == (unsigned)-1)
    {
        printf("fbdev mmap failed\n");
        close(fbfd);
		return -1;
    }
```

## 3. 프레임 버퍼 수정하기

주로 `fb_write( )` 함수를 이용하여 프레임 버퍼를 수정하였다.
이 때, 주의 해야 할 점은 R-G-B순서로 인코딩되는 Bitmap과 달리 프레임버퍼는 0-R-G-B로 인코딩되기 때문에 bit연산을 통해 순서를 바꿔주었다.

```
void fb_write(char* picData, int picWidth, int picHeight)
{
	int coor_y=0;
	int coor_x=0;
	int targetHeight = (fbHeight<picHeight)?fbHeight:picHeight;	//if Screen과 파일 사이즈가 안맞으면
	int targetWidth = (fbWidth<picWidth)?fbWidth:picWidth;		//if Screen과 파일 사이즈가 안맞으면

	for(coor_y = 0; coor_y < targetHeight; coor_y++)
	{
		int bmpYOffset = coor_y*picWidth*3; ///Every 1Pixel requires 3Bytes.
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
}
```

## 4. 백그라운드 그리기

이제 LCD화면을 수정해야할 준비는 끝났다.
제일 먼저, 기본적인 Background 화면을 구성하려고 했다.
화면 구성에 필요했던 것은 X,Y축과 표시할 그래프에 대한 정보이다.

X,Y축의 경우, 해당 그려야할 위치를 구성하는 것은 편했다.

```
    // draw Y-axis
    for(int i=0;i<HEIGHT;i++)
    {
        data[((WIDTH*i)+WIDTH-3)*3+0] = 0xff;
        data[((WIDTH*i)+WIDTH-3)*3+1] = 0xff;
        data[((WIDTH*i)+WIDTH-3)*3+2] = 0xff;
    }

    // draw X-axis
    for(int i=3;i<WIDTH;i++)
    {
        data[((WIDTH*(HEIGHT/2))+i)*3+0] = 0xff;
        data[((WIDTH*(HEIGHT/2))+i)*3+1] = 0xff;
        data[((WIDTH*(HEIGHT/2))+i)*3+2] = 0xff;
    }

```

하지만 우측 하단의 표시되는 그래프의 정보를 표현하기 위해서는
다양한 문자의 표현을 원하는 위치에 작업하기 위해 조금의 노가다성 작업이 필요했다.

## 5. 실시간 그래프 정보 표시

그래프를 그리기 위한 함수의 동작 방식은 다음과 같이 설계했다.

1. 현재 점(point 1)의 위치를 받는다.
   => accel_ex 에 저장
2. 약간의 딜레이 후, 다음 점(point 2)의 위치를 받는다.
   => temp_ex에 저장

> > 값들을 초기화 할 때, point2에서 다시 시작하기위해 값을 저장해놓아야 한다.
> > => 가장 처음 그리는 점 이후에는 temp_ex(point 2)와 새로 받는 값의 순환임으로
> > temp_ex(point 2)를 point1에 저장하고, temp_ex(point 2)에 새로운 값을 받아오는 방식으로 구현했다.

    if(get_data==1)
    {
    	// Get First Data
    	accel_data = (int *)shmemAddr;
    	//printf("accel_data: %d %d %d\r\n",accel_data[1],accel_data[2],accel_data[3]);

    	if(((accel[0] = 300 - accel_data[0]/10) < 0)
    		|| ((accel[0] = 300 - accel_data[0]/10) > 600))
    	{
    		if(accel[0] < 300) accel[0]= 0;
    		else accel[0] = 600;
    	}
    	if(((accel[1] = 300 - accel_data[1]/10) < 0)
    		|| ((accel[1] = 300 - accel_data[1]/10) > 600))
    	{
    		if(accel[1] < 300) accel[1]= 0;
    		else accel[1] = 600;
    	}
    	if(((accel[2] = 300 - accel_data[2]/10) < 0)
    		|| ((accel[2] = 300 - accel_data[2]/10) > 600))
    	{
    		if(accel[2] < 300) accel[2]= 0;
    		else accel[2] = 600;
    	}

    	if(first_data==1) {
    		accel_ex[0] = accel[0];
    		accel_ex[1] = accel[1];
    		accel_ex[2] = accel[2];
    		first_data=0;
    	}
    	else {
    		accel_ex[0] = temp_ex[0];
    		accel_ex[1] = temp_ex[1];
    		accel_ex[2] = temp_ex[2];
    	}

    	usleep(10);

    	// Get Second Data
        accel_data = (int *)shmemAddr;
    	//printf("accel_data: %d %d %d\r\n",accel_data[0],accel_data[1],accel_data[2]);
    	if(	RxButton.keyInput==5 ) pthread_cancel(thread_t2);
    	if(((accel[0] = 300 - accel_data[0]/10) < 0)
    		|| ((accel[0] = 300 - accel_data[0]/10) > 600))
    	{
    		if(accel[0] < 300) accel[0]= 0;
    		else accel[0] = 600;
    	}
    	if(((accel[1] = 300 - accel_data[1]/10) < 0)
    		|| ((accel[1] = 300 - accel_data[1]/10) > 600))
    	{
    		if(accel[1] < 300) accel[1]= 0;
    		else accel[1] = 600;
    	}
    	if(((accel[2] = 300 - accel_data[2]/10) < 0)
    		|| ((accel[2] = 300 - accel_data[2]/10) > 600))
    	{
    		if(accel[2] < 300) accel[2]= 0;
    		else accel[2] = 600;
    	}

    	temp_ex[0] = accel[0];
    	temp_ex[1] = accel[1];
    	temp_ex[2] = accel[2];
    	//printf("\t accel: %d %d %d\r\n",accel[0],accel[1],accel[2]);
    	get_data=0;
    }

3. 두 점을 연결한 후, 값들을 초기화한다.
   => get_data가 0이 되면 draw 부분을 실행한다.

```
    // draw x line
    data[((WIDTH*(accel_ex[0]+x))+WIDTH-cnt)*3+0] = 0xff;
    data[((WIDTH*(accel_ex[0]+x))+WIDTH-cnt)*3+1] = 0x00;
    data[((WIDTH*(accel_ex[0]+x))+WIDTH-cnt)*3+2] = 0x00;
    x_temp[cnt-3] = ((WIDTH*(accel_ex[0]+x))+WIDTH-cnt)*3;

	// draw y line
    data[((WIDTH*(accel_ex[1]+y))+WIDTH-cnt)*3+0] = 0x00;
    data[((WIDTH*(accel_ex[1]+y))+WIDTH-cnt)*3+1] = 0x00;
    data[((WIDTH*(accel_ex[1]+y))+WIDTH-cnt)*3+2] = 0xff;
    y_temp[cnt-3] = ((WIDTH*(accel_ex[1]+y))+WIDTH-cnt)*3;

	// draw z line
    data[((WIDTH*(accel_ex[2]+z))+WIDTH-cnt)*3+0] = 0x00;
    data[((WIDTH*(accel_ex[2]+z))+WIDTH-cnt)*3+1] = 0xff;
    data[((WIDTH*(accel_ex[2]+z))+WIDTH-cnt)*3+2] = 0x00;
    z_temp[cnt-3] = ((WIDTH*(accel_ex[2]+z))+WIDTH-cnt)*3;

    if((accel[0]+x) > accel_ex[0]) x-=1;
	else if((accel[0]+x) < accel_ex[0]) x+=1;
	else reachPos_xyz[0]=1;

	if((accel[1]+y) > accel_ex[1]) y-=1;
	else if((accel[1]+y) < accel_ex[1]) y+=1;
	else reachPos_xyz[1]=1;

	if((accel[2]+z) > accel_ex[2]) z-=1;
	else if((accel[2]+z) < accel_ex[2]) z+=1;
	else reachPos_xyz[2]=1;

    fb_write(data);


    if(is_reach_pos(reachPos_xyz))
    {
		get_data=1;
		reachPos_xyz[0]=0;
		reachPos_xyz[1]=0;
		reachPos_xyz[2]=0;

		x=0;y=0;z=0;
		cnt++;
	}
```

4.
