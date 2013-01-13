#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/hidraw.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

void bailoutUinputConfig()
{
	perror("Error while configuring uinput device: ");
	exit(2);
}

void sendKeyPress(int uinputfd, int keyCode)
{
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

	ev.type = EV_KEY;
	ev.code = keyCode;
	ev.value = 1;

	int ret = write(uinputfd, &ev, sizeof(ev));
	if(ret < 0)
		fprintf(stderr, "Error while sending event");

	ev.type = EV_KEY;
	ev.code = keyCode;
	ev.value = 0;

	ret = write(uinputfd, &ev, sizeof(ev));
	if(ret < 0)
		fprintf(stderr, "Error while sending event");

	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;

	ret = write(uinputfd, &ev, sizeof(ev));
	if(ret < 0)
		fprintf(stderr, "Error while sending event");
}

void handleVolumeDown(int uinputfd)
{
	printf("Down\n");
	sendKeyPress(uinputfd, KEY_VOLUMEDOWN);
}

void handleVolumeUp(int uinputfd)
{
	printf("Up\n");
	sendKeyPress(uinputfd, KEY_VOLUMEUP);
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		fprintf(stderr,"Usage: %s /dev/hidrawN\n", argv[0]);
		return 1;
	}

	//Open the HID device
	int hidfd = open(argv[1], O_RDWR);
	if(hidfd < 0)
	{
		perror("Opening hidraw device: ");
		return 2;
	}

	//Get HID device info
	struct hidraw_devinfo devInfo;
	ioctl(hidfd, HIDIOCGRAWINFO, &devInfo);
	printf("VENDOR: %x, PRODUCT: %x\n", devInfo.vendor, devInfo.product);
	char nameBuffer[256];
	ioctl(hidfd, HIDIOCGRAWNAME(256), nameBuffer);
	printf("NAME: %s\n", nameBuffer);

	//Create UInput device
	int uinputfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if(uinputfd < 0)
	{
		perror("Opening uinput device (is kernel module loaded?): ");
		return 2;
	}

	//Configure it
	int ret=ioctl(uinputfd, UI_SET_EVBIT, EV_KEY);
	if(ret < 0)
		bailoutUinputConfig();
	ret=ioctl(uinputfd, UI_SET_EVBIT, EV_SYN);
	if(ret < 0)
		bailoutUinputConfig();
	ret=ioctl(uinputfd, UI_SET_KEYBIT, KEY_MUTE);
	if(ret < 0)
		bailoutUinputConfig();
	ret=ioctl(uinputfd, UI_SET_KEYBIT, KEY_VOLUMEDOWN);
	if(ret < 0)
		bailoutUinputConfig();
	ret=ioctl(uinputfd, UI_SET_KEYBIT, KEY_VOLUMEUP);
	if(ret < 0)
		bailoutUinputConfig();

	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));

	strncpy(uidev.name, nameBuffer, UINPUT_MAX_NAME_SIZE);
	uidev.id.bustype = devInfo.bustype;
	uidev.id.vendor  = devInfo.vendor;
	uidev.id.product = devInfo.product;
	uidev.id.version = 1;

	ret = write(uinputfd, &uidev, sizeof(uidev));
	if(ret < 0 || ret!=sizeof(uidev))
		bailoutUinputConfig();

	ret = ioctl(uinputfd, UI_DEV_CREATE);
	if(ret < 0)
		bailoutUinputConfig();

	ret=sendGlobalConfPacket(hidfd,
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
	bool buttonPressed = false;
	while(1)
	{
		char buf[16];
		int ret=read(hidfd, buf, 16);
		if(ret<0)
		{
			perror("Failure: ");
			return 2;
		}
		//Handle wheel control
		int newWheelPos = wheelPosMap[buf[6]&3];
		if(wheelPos != -1 && wheelPos!=newWheelPos)
		{
			//Measure the direction of the rotation
			if(((wheelPos+1)%4) == newWheelPos)
				handleVolumeUp(uinputfd);
			else if(((wheelPos-1+4)%4) == newWheelPos)
				handleVolumeDown(uinputfd);
			else
				fprintf(stderr, "Invalid position variation\n");
		}
		wheelPos = newWheelPos;
		//Handle button control
		if((buf[6] & 4) && buttonPressed==false)
			printf("Mute\n");
		buttonPressed = (wheelPos >> 2);
	}
}
