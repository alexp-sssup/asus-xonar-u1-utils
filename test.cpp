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
	ret=libusb_reset_device(dev);
	assert(ret>=0);

//	ret=libusb_set_configuration(dev,1);
//	assert(ret>=0);

//	ret=libusb_claim_interface(dev,0);
//	assert(ret>=0);
//	ret=libusb_claim_interface(dev,1);
//	assert(ret>=0);
//	ret=libusb_claim_interface(dev,2);
//	assert(ret>=0);
	ret=libusb_claim_interface(dev,3);
	assert(ret>=0);

	/*ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_OTHER,
			LIBUSB_REQUEST_SET_FEATURE,0x0004,0x2,NULL,0,0);
	assert(ret==0);
	unsigned char status[4];
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_OTHER,
			LIBUSB_REQUEST_GET_STATUS,0x0000,0x2,status,4,0);
	assert(ret==0);
	printf("STATUS ");
	for(int i=0;i<ret;i++)
		printf("%02x ",status[i]);
	printf("\n");
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_OTHER,
			LIBUSB_REQUEST_CLEAR_FEATURE,0x0014,0x2,NULL,0,0);
	assert(ret==0);*/

/*	libusb_device_descriptor desc;
	ret=libusb_get_device_descriptor(libusb_get_device(dev),&desc);
	assert(ret>=0);
	//Print in chars
	unsigned char* str=(unsigned char*)&desc;
	for(int i=0;i<sizeof(libusb_device_descriptor);i++)
		printf("%02x ",str[i]);
	printf("\n");

	libusb_config_descriptor* config;
	ret=libusb_get_config_descriptor(libusb_get_device(dev),0,&config);
	assert(ret>=0);
	//Print in chars
	str=(unsigned char*)config;
	for(int i=0;i<sizeof(libusb_config_descriptor);i++)
		printf("%02x ",str[i]);
	printf("\n");
	libusb_free_config_descriptor(config);*/

	//OK
	
	/*unsigned char buf[0x100];
	ret=libusb_get_string_descriptor(dev, 0, 0, buf, 255);
	assert(ret>=0);
	for(int i=0;i<ret;i++)
		printf("%02x ",buf[i]);
	printf("\n");
	ret=libusb_get_string_descriptor(dev, 1, 0x409, buf, 255);
	assert(ret>=0);
	for(int i=0;i<ret;i++)
		printf("%02x ",buf[i]);
	printf("\n");*/

	ret=libusb_set_interface_alt_setting(dev, 1, 0);
	assert(ret>=0);
	ret=libusb_set_interface_alt_setting(dev, 2, 0);
	assert(ret>=0);

	//classRequestOut0(dev, 0xA, 0x0, 0x3);

	//OK

	unsigned char buf[0x72];
	/*classRequestOut0(dev,0xA,0x00,0x03);
	ret=libusb_get_string_descriptor(dev, 0x50, 0x409, buf, 0x68);
	printf("%i\n",ret);
	assert(ret>=0);
	for(int i=0;i<ret;i++)
		printf("%02x ",buf[i]);
	printf("\n");*/
	/*vendorRequestIn(dev,0,0x17);
	vendorRequestIn(dev,0,0x18);
	vendorRequestOut(dev,0x8,0x4c,0x40);
	vendorRequestOut(dev,0x8,0x4c,0x00);
	vendorRequestOut(dev,0x8,0x40,0xa1);
	vendorRequestOut(dev,0x8,0x41,0x00);
	vendorRequestOut(dev,0x8,0x4b,0x02);
	vendorRequestOut(dev,0x8,0x4d,0x01);
	vendorRequestOut(dev,0x8,0x4c,0x82);
	vendorRequestIn(dev,0,0x4c);
	vendorRequestIn(dev,0,0x43);
	vendorRequestIn(dev,0,0x44);
	vendorRequestOut(dev,0x8,0x40,0xa1);
	vendorRequestOut(dev,0x8,0x41,0xd6);
	vendorRequestOut(dev,0x8,0x4b,0x04);
	vendorRequestOut(dev,0x8,0x4d,0x01);
	vendorRequestOut(dev,0x8,0x4c,0x82);
	vendorRequestIn(dev,0,0x4c);
	vendorRequestIn(dev,0,0x43);
	vendorRequestIn(dev,0,0x44);
	vendorRequestIn(dev,0,0x45);
	vendorRequestIn(dev,0,0x46);
	vendorRequestOut(dev,0x8,0x5,0x00);
	vendorRequestOut(dev,0x8,0x5,0x00);
	vendorRequestOut(dev,0x8,0x0,0x01);
	vendorRequestOut(dev,0x8,0x5,0x00);
	vendorRequestOut(dev,0x8,0x7,0x20);
	vendorRequestOut(dev,0x8,0x8,0x00);
	vendorRequestOut(dev,0x8,0x27,0x00);
	vendorRequestOut(dev,0x8,0x28,0x40);
	vendorRequestOut(dev,0x8,0x30,0x24);
	vendorRequestOut(dev,0x8,0x31,0x00);
	vendorRequestOut(dev,0x8,0x32,0x04);
	vendorRequestOut(dev,0x8,0x37,0x00);
	vendorRequestOut(dev,0x8,0x38,0x00);
	vendorRequestOut(dev,0x8,0x39,0x49);
	vendorRequestOut(dev,0x8,0x3a,0x40);
	classRequestOut1(dev,0x1,0x0,2048,0x1);
	classRequestOut2(dev,0x1,0x201,0xb00,0x00,0x00);
	classRequestOut2(dev,0x1,0x202,0xb00,0x00,0x00);
	classRequestOut2(dev,0x1,0x201,0xa00,0x00,0xf5);
	classRequestOut2(dev,0x1,0x202,0xa00,0x00,0xf5);
	classRequestOut1(dev,0x1,0xa00,0x900,0x00);
	//OK
	classRequestOut1(dev,0x1,0x100,2304,0x00);
	classRequestOut2(dev,0x1,0x201,2304,0x00,0x00);
	classRequestOut2(dev,0x1,0x202,2304,0x00,0x00);
	classRequestOut2(dev,0x1,0x201,2816,0x00,0xf5);
	classRequestOut2(dev,0x1,0x202,2816,0x00,0xf5);
	classRequestOut2(dev,0x1,0x201,2560,0x00,0x05);
	classRequestOut2(dev,0x1,0x202,2560,0x00,0x05);
	classRequestOut1(dev,0x1,0x000,2048,0x01);

	vendorRequestOut(dev,0x8,0x05,0x00);
	vendorRequestOut(dev,0x8,0x32,0x04);
	vendorRequestOut(dev,0x8,0x27,0x00);
	vendorRequestOut(dev,0x8,0x32,0x06);*/
	
	ret=libusb_set_interface_alt_setting(dev, 1, 1);
	assert(ret>=0);

	//classRequestOut3(dev,0x1,0x100,1,0x80,0xbb,0x00);
	//vendorRequestOut(dev,0x8,0x32,0x6);

	unsigned char reportOut1[] = {0x02,0x01,0x80,0x80,0x00,0x00,0x00,0x58,0x08,0x20,0x00,0x9c,0x06,0x00,0x00,0x0e};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut1,16,0);
	assert(ret==16);
	unsigned char reportOut2[] = {0x11,0x02,0x00,0x00,0x30,0x0f,0x02,0x94,0x13,0x00,0x10,0x00,0x00,0x00,0x00,0x00};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut2,16,0);
	assert(ret==16);
	unsigned char reportOut3[] = {0x02,0x01,0x84,0x04,0x00,0x00,0x00,0x70,0x13,0x00,0x10,0xf0,0x12,0x00,0x10,0x0f};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut3,16,0);
	assert(ret==16);
	unsigned char reportOut4[] = {0x02,0x01,0x84,0x04,0x00,0x00,0x00,0x70,0x13,0x00,0x10,0xf0,0x12,0x00,0x10,0x0f};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut3,16,0);
	assert(ret==16);
	unsigned char reportOut5[] = {0x13,0x02,0x45,0x89,0x13,0x00,0x10,0x94,0x13,0x00,0x10,0xb8,0x0b,0x00,0x00,0xdc};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut5,16,0);
	assert(ret==16);
	unsigned char reportOut6[] = {0x02,0x01,0x94,0x10,0x00,0x00,0x00,0x70,0x13,0x00,0x10,0xf0,0x12,0x00,0x10,0x0f};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut6,16,0);
	assert(ret==16);
	unsigned char reportOut7[] = {0x02,0x01,0x9c,0x08,0x00,0x00,0x00,0x70,0x13,0x00,0x10,0xf0,0x12,0x00,0x10,0x0f};
	ret=libusb_control_transfer(dev,LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,0x9,0x200,3,
			reportOut7,16,0);
	assert(ret==16);

	ret=libusb_set_interface_alt_setting(dev, 1, 0);
	assert(ret>=0);

	//vendorRequestOut(dev,0x8,0x32,0x07);

	int transferred;
	ret=libusb_interrupt_transfer(dev, 0x87, buf, 20, &transferred, 0);
	assert(ret>=0);
	printf("RET: %i\n",transferred);
}
