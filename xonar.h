//Setup of controls and leds
//	bit 7 -> Enable interrupt to report the control status
//	bit 6-5 -> Unknown/nothing
//	bit 4-3 -> 11: Blue LED blinks, 00/01/10: Blue LED always on
//	bit 2-1 -> 11: Red LED blinks always, 10: Red LED always off, 00/01: Red LED blinks on audio activity
//	bit 0 -> Unknown/nothing
#define ASUS_XONAR_U1_ENABLE_INTERRUPT (1<<7)
#define ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING (3<<3)
#define ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING (3<<1)
#define ASUS_XONAR_U1_DISABLE_RED_LED (2<<1)

#define ASUS_XONAR_U1_BLUE_LED 0x13
#define ASUS_XONAR_U1_RED_LED 0x11
