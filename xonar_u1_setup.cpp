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
 
 
 #define CTRL_IN                LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE 
 #define VENDOR_CTRL_IN         LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_INTERFACE 
 #define CTRL_OUT       LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE 

void vendorRequestIn(libusb_device_handle* dev, int request, int value)
{
	unsigned char buf[20];
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_OTHER,request,value,0,buf,1,0);
	printf("VI: R: %02x, V: %02x -> %02x\n",request,value,buf[0]);
	assert(ret==1);
}

void vendorRequestOut(libusb_device_handle* dev, int request, int value, unsigned char v)
{
	unsigned char buf[20];
	buf[0]=v;
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_OTHER,request,value,0,buf,1,0);
	assert(ret==1);
}

void classRequestOut0(libusb_device_handle* dev, int request, int value, int index)
{
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,request,value,index,NULL,0,0);
	assert(ret==0);
}

void classRequestOut1(libusb_device_handle* dev, int request, int value, int index, unsigned char v)
{
	unsigned char buf[20];
	buf[0]=v;
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,request,value,index,buf,1,0);
	assert(ret==1);
}

void classRequestOut2(libusb_device_handle* dev, int request, int value, int index, unsigned char v1, unsigned char v2)
{
	unsigned char buf[20];
	buf[0]=v1;
	buf[1]=v2;
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,request,value,index,buf,2,0);
	assert(ret==2);
}

void classRequestOut3(libusb_device_handle* dev, int request, int value, int index, unsigned char v1, unsigned char v2, unsigned char v3)
{
	unsigned char buf[20];
	buf[0]=v1;
	buf[1]=v2;
	buf[2]=v3;
	int ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_ENDPOINT,request,value,index,buf,3,0);
	assert(ret==3);
}

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
	//Byte 2 is a bitfield use to setup the control and leds
	//	-> 0x80: Enable interrupt to report the control status
	//	-> 0x18: Enable the blinking patterns
	//	-> TODO: investigate lower bits
	unsigned char setupReport[] = {0x02,0x01,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			setupReport,16,0);
	assert(ret==16);
	//This next two reports set the blinking duty cycle for the red and blue leds
	//Byte 0 is 0x11 for red led and 0x13 for blue led
	//Byte 1 is unknown
	//Byte 2 is time spent off
	//Byte 3 is total duty cycle time (TODO: understand unit of time)
	unsigned char reportOut5[] = {0x13,0x02,0x89,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
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
