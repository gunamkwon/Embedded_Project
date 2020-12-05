all: main.elf

main.elf: main.c libMyPeri.a
	arm-linux-gnueabi-gcc main.c -o main.elf -lpthread -lMyPeri -L. -lm
libMyPeri.a: led.o button.o buzzer.o fnd.o textlcd.o colorled.o temp.o AMGsensor.o
	arm-linux-gnueabi-ar rc libMyPeri.a led.o button.o buzzer.o fnd.o textlcd.o colorled.o temp.o AMGsensor.o
AMGsensor.o: AMGsensor.c AMGsensor.h
	arm-linux-gnueabi-gcc -c AMGsensor.c -o AMGsensor.o
temp.o: temp.c temp.h
	arm-linux-gnueabi-gcc -c temp.c -o temp.o
colorled.o: colorled.c colorled.h
	arm-linux-gnueabi-gcc -c colorled.c -o colorled.o
textlcd.o: textlcd.c textlcd.h
	arm-linux-gnueabi-gcc -c textlcd.c -o textlcd.o
fnd.o: fnd.c fnd.h
	arm-linux-gnueabi-gcc -c fnd.c -o fnd.o
buzzer.o: buzzer.c buzzer.h
	arm-linux-gnueabi-gcc -c buzzer.c -o buzzer.o
button.o: button.c button.h
	arm-linux-gnueabi-gcc -c button.c -o button.o
led.o: led.c led.h
	arm-linux-gnueabi-gcc -c led.c -o led.o



clean:
	rm *.o *.a
