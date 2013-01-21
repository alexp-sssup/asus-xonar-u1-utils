/*
 * Xonard
 *
 * Copyright 2012-2013 Alessandro Pignotti
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xonard.h"

static const char wheelPosMap[4] = { 0, 1, 3, 2 };

int sendGlobalConfPacket(int hidfd, uint8_t conf)
{
	uint8_t buf[17];
	memset(buf,0,17);
	//The device does not use report numbers. So first byte is 0
	buf[0] = 0x00;
	//The configuration command is 0x02
	buf[1] = 0x02;
	//Next byte has unknown meaning
	buf[2] = 0x01;
	//Next byte is the requested configuration
	buf[3] = conf;
	int ret=write(hidfd, buf, 17);
	if(ret<0)
		return ret;
	if(ret!=17)
    {
        fprintf(stderr, "Failure while writing global configuration packet\n");
        return 1;
    }
	return 0;
}

int sendBlinkConfPacket(int hidfd, uint8_t ledIndex, uint8_t dutyCycleA, uint8_t dutyCycleTotal)
{
	uint8_t buf[17];
	memset(buf,0,17);
	//The device does not use report numbers. So first byte is 0
	buf[0] = 0x00;
	//The command depends on the led
	buf[1] = ledIndex;
	//Next byte has unknown meaning
	buf[2] = 0x02;
	//Next two bytes configures the duty cycle
	buf[3] = dutyCycleA;
	buf[4] = dutyCycleTotal;
	int ret=write(hidfd, buf, 17);
	if(ret<0)
		return ret;
	if(ret!=17)
    {
		fprintf(stderr, "Failure while writing blinking configuration packet\n");
        return 1;
    }
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
	sendKeyPress(uinputfd, KEY_VOLUMEDOWN);
}

void handleVolumeUp(int uinputfd)
{
	sendKeyPress(uinputfd, KEY_VOLUMEUP);
}

void handleMute(int uinputfd)
{
	sendKeyPress(uinputfd, KEY_MUTE);
}

int main(int argc, char* argv[])
{
	int ret;
	if(argc!=2)
	{
		fprintf(stderr,"Usage: %s /dev/hidrawN\n", argv[0]);
		return 1;
	}
	//Daemonize
	pid_t pid=fork();
	if(pid < 0)
	{
		perror("Error while forking: ");
		return 2;
	}
	if(pid > 0)
	{
		//Close the parent process
		return 0;
	}
	//Child process
	umask(0022);
	pid_t sid=setsid();
	if(sid < 0)
	{
		perror("Error while creating session: ");
		return 2;
	}

	ret=chdir("/tmp");
	if(ret < 0)
	{
		perror("Error while changing directory: ");
		return 2;
	}

	//Open the HID device
	int hidfd = open(argv[1], O_RDWR);
	if(hidfd < 0)
	{
		perror("Error opening hidraw device: ");
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
		perror("Error opening uinput device (is kernel module loaded?): ");
		return 2;
	}

	//Configure it
	ret=ioctl(uinputfd, UI_SET_EVBIT, EV_KEY);
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

    //Send a sane LED configuration
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
	int buttonPressed = 0;
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
		if((buf[6] & 4) && buttonPressed==0)
			handleMute(uinputfd);
		buttonPressed = (buf[6] & 4) >> 2;
	}
}
