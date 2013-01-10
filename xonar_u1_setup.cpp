#include <libusb.h>
#include <stdio.h>
#include <assert.h>

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

//Setup of controls and leds
//	bit 7 -> Enable interrupt to report the control status
//	bit 6-5 -> Unknown/nothing
//	bit 4-3 -> 11: Blue LED blinks, 00/01/10: Blue LED always on
//	bit 2-1 -> 11: Red LED blinks always, 10: Red LED always off, 00/01: Red LED blinks on audio activity
//	bit 0 -> Unknown/nothing
#define ASUS_XONAR_U1_ENABLE_INTERRUPT (1<<7)
#define ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING (3<<3)
#define ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING (3<<1)

int main()
{
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
		ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING
			,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			setupReport,16,0);
	assert(ret==16);
	//This next two reports set the blinking duty cycle for the red and blue leds
	//Byte 0 is 0x11 for red led and 0x13 for blue led
	//Byte 1 is unknown
	//Byte 2 is time spent off
	//Byte 3 is total duty cycle time (TODO: understand unit of time)
	unsigned char reportOut5[] = {0x13,0x02,0x45,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut5,16,0);
	assert(ret==16);

	unsigned char redOut1[] = {0x11,0x02,0x45,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			redOut1,16,0);
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
