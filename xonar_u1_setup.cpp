#include <libusb.h>
#include <stdio.h>
#include <assert.h>

#include "xonar.h"
 #define HID_GET_REPORT                0x01 
 #define HID_GET_IDLE                  0x02 
 #define HID_GET_PROTOCOL              0x03 
 #define HID_SET_REPORT                0x09 
 #define HID_SET_IDLE                  0x0A 
 #define HID_SET_PROTOCOL              0x0B 
 #define HID_REPORT_TYPE_INPUT         0x01 
 #define HID_REPORT_TYPE_OUTPUT        0x02 
 #define HID_REPORT_TYPE_FEATURE       0x03 
 
 
 #define CTRL_OUT       LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE 

int main()
{
	return 0;
	libusb_init(NULL);
	libusb_set_debug(NULL,3);
	libusb_device_handle* dev=libusb_open_device_with_vid_pid (NULL, 0x0b05, 0x1743);

	int ret;
	//First release the kernel drivers
	bool driverWasActive;
	driverWasActive=libusb_kernel_driver_active(dev, 3);
	if(driverWasActive)
	{
		ret=libusb_detach_kernel_driver(dev, 3);
		assert(ret>=0);
	}

	ret=libusb_claim_interface(dev,3);
	assert(ret>=0);

	//The first report initializes the blinking
	//Byte 0 is a command
	//Byte 1 us unknown
	//Byte 2 is a bitfield use to setup the controls and leds
	unsigned char setupReport[] = {0x02,0x01,
		ASUS_XONAR_U1_ENABLE_INTERRUPT|
		ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING|
		ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	//This next two reports set the blinking duty cycle for the red and blue leds
	//Byte 0 is 0x11 for red led and 0x13 for blue led
	//Byte 1 is unknown
	//Byte 2 is time spent off for blue and time spent on for red
	//Byte 3 is total duty cycle time (The time unit seems to be around 20 ms)
	unsigned char blueLedReport[] = {0x13,0x02,0x64,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char redLedReport[] = {0x11,0x02,0x64,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			setupReport,16,0);
	assert(ret==16);
	ret=libusb_control_transfer(dev,CTRL_OUT,HID_SET_REPORT,0x200,3,blueLedReport,16,0);
	assert(ret==16);

	ret=libusb_control_transfer(dev,CTRL_OUT,HID_SET_REPORT,0x200,3,redLedReport,16,0);
	assert(ret==16);

	ret=libusb_release_interface(dev,3);
	assert(ret>=0);

	//Then reattach the audio interfaces, if needed
	if(driverWasActive && !libusb_kernel_driver_active(dev, 3))
	{
		ret=libusb_attach_kernel_driver(dev,3);
		assert(ret>=0);
	}
}
