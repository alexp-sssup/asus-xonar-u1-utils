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

#if ENABLE_CTL == 1
int initIPC()
{
    int sockfd;
    struct sockaddr_un sock;
    //TODO maybe a better and more secure check
    //     if another instance of xonard is launched,
    //     it'll delete its socket and start.
    if (access(SOCK_PATH, F_OK) == 0)
        remove(SOCK_PATH);
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        logErrorMsg("Unable to create socket");
        exit(1);
    }
    memset(&sock, 0, sizeof(struct sockaddr_un));
    sock.sun_family = AF_UNIX;
    snprintf(sock.sun_path, sizeof(sock.sun_path), SOCK_PATH);
    if (bind(sockfd, (struct sockaddr*)&sock, sizeof(struct sockaddr_un)) != 0) {
        logErrorMsg("Unable to bind socket");
        close(sockfd);
        exit(1);
    }
    chmod(SOCK_PATH, SOCK_MODE);
    if (listen(sockfd, 1) != 0) {
        logErrorMsg("Unable to listen socket");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}

void destroyIPC(int sockfd)
{
    if (sockfd > 0) {
        close(sockfd);
        if (!remove(SOCK_PATH)) {
            logErrorMsg("Unable to delete FIFO");
            exit(1);
        }
    }
}

char *getMsg(int sockfd)
{
    char *buffer;
    char frameSize;
    read(sockfd, &frameSize, 1);
    //Skip byte if frame too big
    if (frameSize > MAX_FRAME_SIZE)
        return NULL;
    buffer = (char *)malloc(sizeof(char)*frameSize);
    read(sockfd, buffer, frameSize);
    return buffer;
}

int waitForEvent(int sockfd, int hidfd)
{
    fd_set fd_read;
    struct timeval timeout;
    int fdmax = (sockfd > hidfd) ? sockfd+1 : hidfd+1;
    timeout.tv_sec = 0;
    timeout.tv_usec = 150;
    FD_ZERO(&fd_read);
    FD_SET(sockfd, &fd_read);
    FD_SET(hidfd, &fd_read);
    if (select(fdmax, &fd_read, NULL, NULL, &timeout)) {
        if (FD_ISSET(sockfd, &fd_read)) {
            return sockfd;
        }
        else if (FD_ISSET(hidfd, &fd_read)) {
            return hidfd;
        }
    }
    return -1;
}

int dataReady(int sockfd)
{
    fd_set fd_read;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 150;
    FD_ZERO(&fd_read);
    FD_SET(sockfd, &fd_read);
    if (select(sockfd+1, &fd_read, NULL, NULL, &timeout) != -1) {
        if (FD_ISSET(sockfd, &fd_read)) {
            int bytes;
            ioctl(sockfd, FIONREAD, &bytes);
            if (bytes > 0)
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

void processPacket(int hidfd, char *payload)
{
    if (payload == NULL)
        return;
    if (payload[0] == 0x01) {
        int conf;
        sscanf(payload+1, "%08x", &conf);
        sendGlobalConfPacket(hidfd, conf);
    }
    else if (payload[0] == 0x02) {
        int ledIndex, dutyA, dutyTotal;
        sscanf(payload+1, "%02x%02x%02x", &ledIndex, &dutyA, &dutyTotal);
        sendBlinkConfPacket(hidfd, ledIndex, dutyA, dutyTotal);
    }
}

#endif

void logMsg(char *msg)
{
    syslog(LOG_INFO, msg);
}

void logErrorMsg(char *msg)
{
    syslog(LOG_ERR, "%s: %s", msg, strerror(errno));
}

void logErrorMsgRaw(char *msg)
{
    syslog(LOG_ERR, msg);
}

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
	if(ret!=17)
		logErrorMsgRaw("Failure while writing global configuration packet.");
    else
        return 0;
    return ret;
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
	if(ret!=17)
		logErrorMsgRaw("Failure while writing blinking configuration packet.");
    else
        return 0;
    return ret;
}

void bailoutUinputConfig()
{
	logErrorMsg("Error while configuring uinput device");
	exit(2);
}

void hookKeyEvents(int uinputfd)
{
    int evbit[] = { EV_KEY, EV_SYN };
    int keys[] = { KEY_MUTE, KEY_KATAKANA, KEY_HIRAGANA };
	int ret, i;
    for (i=0; i < 2; ++i)
    {
        ret=ioctl(uinputfd, UI_SET_EVBIT, evbit[i]);
        if (ret < 0)
            bailoutUinputConfig();
    }
    for (i=0; i < 3; ++i)
    {
        ret=ioctl(uinputfd, UI_SET_KEYBIT, keys[i]);
        if (ret < 0)
            bailoutUinputConfig();
    }
}

void sendKeyPress(int uinputfd, int keyCode)
{
	struct input_event ev;
	memset(&ev, 0, sizeof(ev));

    int i;
    for (i=1; i >= 0; --i)
    {
        ev.type = EV_KEY;
        ev.code = keyCode;
        ev.value = i;
        if(write(uinputfd, &ev, sizeof(ev)) < 0)
            logErrorMsgRaw("Error while sending event.");
    }
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;

	if (write(uinputfd, &ev, sizeof(ev)) < 0)
		logErrorMsgRaw("Error while sending event.");
}

void handleVolumeDown(int uinputfd)
{
	sendKeyPress(uinputfd, KEY_KATAKANA);
}

void handleVolumeUp(int uinputfd)
{
	sendKeyPress(uinputfd, KEY_HIRAGANA);
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
    //Syslog starts here
    openlog("xonard", 0, LOG_LOCAL0);
    logMsg("Starting.");
	//Daemonize
	pid_t pid=fork();
	if(pid < 0)
	{
		logErrorMsg("Error while forking");
		return 2;
	}
	else if(pid > 0)
	{
		//Close the parent process
		return 0;
	}
	//Child process
	umask(0022);
	pid_t sid=setsid();
	if(sid < 0)
	{
		logErrorMsg("Error while creating session");
		return 2;
	}

	if(chdir("/tmp") < 0)
	{
		logErrorMsg("Error while changing directory");
		return 2;
	}

    //Close standard fd
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

	//Open the HID device
	int hidfd = open(argv[1], O_RDWR);
	if(hidfd < 0)
	{
		logErrorMsg("Error opening hidraw device");
		return 2;
	}

	//Get HID device info
	struct hidraw_devinfo devInfo;
	ioctl(hidfd, HIDIOCGRAWINFO, &devInfo);
    char logMessage[256];
    //Check if the peripheral is the good one
    if (devInfo.vendor != ASUS_XONAR_VENDOR_ID) {
        snprintf(logMessage, 256, "Wrong vendor id, expected '%x' got '%x'", ASUS_XONAR_VENDOR_ID, devInfo.vendor);
        logErrorMsgRaw(logMessage);
        return 2;
    }
    if (devInfo.product != ASUS_XONAR_PRODUCT_ID) {
        snprintf(logMessage, 256, "Wrong product id, expected '%x' got '%x'", ASUS_XONAR_PRODUCT_ID, devInfo.product);
        logErrorMsgRaw(logMessage);
        return 2;
    }
    snprintf(logMessage, 256, "VENDOR: %x, PRODUCT: %x", devInfo.vendor, devInfo.product);
	logMsg(logMessage);
	char nameBuffer[256];
	ioctl(hidfd, HIDIOCGRAWNAME(256), nameBuffer);
    snprintf(logMessage, 256, "NAME: %s", nameBuffer);
	logMsg(logMessage);

    //If enabled starts the IPC for ctl
#if ENABLE_CTL == 1
    int sockfd = initIPC();
    if (sockfd < 0)
    {
        return 3;
    }
#endif

	//Create UInput device
	int uinputfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if(uinputfd < 0)
	{
		logErrorMsg("Error opening uinput device (is kernel module loaded?)");
		return 2;
	}

	//Configure it
    hookKeyEvents(uinputfd);

    //Create the device
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
#if ENABLE_CTL == 1
        int retfd = waitForEvent(sockfd, hidfd);
        if (retfd == sockfd) {
            int connfd = accept(sockfd, NULL, NULL);
            if (connfd > -1) {
                while (dataReady(connfd)) {
                    char *sockbuf = getMsg(connfd);
                    if (sockbuf) {
                        processPacket(hidfd, sockbuf);
                        free(sockbuf);
                    }
                }
                close(connfd);
            }
        }
        else if (retfd == hidfd) {
#endif
		int ret=read(hidfd, buf, 16);
		if(ret<0)
		{
			logErrorMsg("Failure");
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
				logErrorMsgRaw("Invalid position variation");
		}
		wheelPos = newWheelPos;
		//Handle button control
		if((buf[6] & 4) && buttonPressed==0)
			handleMute(uinputfd);
		buttonPressed = (buf[6] & 4) >> 2;
	}
#if ENABLE_CTL == 1
    }
#endif
}
