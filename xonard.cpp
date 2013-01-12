#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "xonar.h"

static const char wheelPosMap[4] = { 0, 1, 3, 2 };

int sendGlobalConfPacket(int hidfd, uint8_t conf)
{
	uint8_t buf[17];
	memset(buf,0,17);
	//The device does not use report numbers. So first byte is 0
	buf[0] = 0x00;
	//The configuration command is 0x02
	buf[1] = 0x02;
	//Next byte as unkown meaning
	buf[2] = 0x01;
	//Next byte is the requested configuration
	buf[3] = conf;
	int ret=write(hidfd, buf, 17);
	if(ret<0)
		return ret;
	if(ret!=17)
		fprintf(stderr, "Failure while writing global configuration packet\n");
	return 0;
}

/*
 * Both dutyCycleA and dutyCycleTotal are in 20ms units (more or less)
 * dutyCycleA is the time spent OFF for the blue led
 * 		 the time spend ON for the red led
 */
int sendBlinkConfPacket(int hidfd, uint8_t ledIndex, uint8_t dutyCycleA, uint8_t dutyCycleTotal)
{
	uint8_t buf[17];
	memset(buf,0,17);
	//The device does not use report numbers. So first byte is 0
	buf[0] = 0x00;
	//The command depends on the led
	buf[1] = ledIndex;
	//Next byte as unkown meaning
	buf[2] = 0x02;
	//Next two bytes are configures the duty cycle
	buf[3] = dutyCycleA;
	buf[4] = dutyCycleTotal;
	int ret=write(hidfd, buf, 17);
	if(ret<0)
		return ret;
	if(ret!=17)
		fprintf(stderr, "Failure while writing blinking configuration packet\n");
	return 0;
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		fprintf(stderr,"Usage: %s /dev/hidrawN", argv[0]);
		return 1;
	}

	int hidfd = open(argv[1], O_RDWR);
	if(hidfd < 0)
	{
		perror("Opening hidraw device: ");
		return 2;
	}
	int ret=sendGlobalConfPacket(hidfd,
			ASUS_XONAR_U1_ENABLE_INTERRUPT|
			ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING|
			ASUS_XONAR_U1_DISABLE_RED_LED);
	assert(ret==0);
	ret=sendBlinkConfPacket(hidfd, ASUS_XONAR_U1_BLUE_LED, 100, 200);
	assert(ret==0);
	//Adding one to the time ON eliminates flickering
	ret=sendBlinkConfPacket(hidfd, ASUS_XONAR_U1_RED_LED, 101, 100);
	assert(ret==0);

	// Initialize to -1, we need to set it to a sane value when the first event is received
	int wheelPos = -1;
	int buttonPressed = 0;
	while(1)
	{
		char buf[16];
		int ret=read(hidfd, buf, 16);
		//Handle wheel control
		int newWheelPos = wheelPosMap[buf[6]&3];
		if(wheelPos != -1)
		{
			if(newWheelPos < wheelPos)
				printf("Down\n");
			else if(newWheelPos > wheelPos)
				printf("Up\n");
		}
		wheelPos = newWheelPos;
		//Handle button control
		//if(wheelPos & 3

	//	printf("%u\n",keyState);
	}
}
