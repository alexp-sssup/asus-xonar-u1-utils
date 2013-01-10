#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "xonar.h"

static const char wheelPosMap[4] = { 0, 1, 3, 2 };

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
	unsigned char setupReport[] = {0x00,0x02,0x01,
		ASUS_XONAR_U1_ENABLE_INTERRUPT|
		ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING|
		ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char blueLedReport[] = {0x00,0x13,0x02,0x64,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char redLedReport[] = {0x00,0x11,0x02,0x64,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	printf("INIT1\n");
	int ret=write(hidfd, setupReport, 17);
	assert(ret==17);
	printf("INIT2\n");
	ret=write(hidfd, blueLedReport, 17);
	assert(ret==17);
	printf("INIT3\n");
	ret=write(hidfd, redLedReport, 17);
	assert(ret==17);

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
