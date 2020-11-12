all:libMyPeri.a

libMyPeri.a: led.o button.o buzzer.o
	arm-linux-gnueabi-ar rc libMyPeri.a led.o button.o buzzer.o
buzzer.o: buzzer.c buzzer.h
	arm-linux-gnueabi-gcc -c buzzer.c -o buzzer.o
button.o: button.c button.h
	arm-linux-gnueabi-gcc -c button.c -o button.o
led.o: led.c led.h
	arm-linux-gnueabi-gcc -c led.c -o led.o



clean:
	rm *.o *.a
