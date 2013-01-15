#!/bin/sh

#Asus Xonar U1 does not preserve the configuration on hibernate
#We will manully unbind the usbhid driver from the device and
#bind it again on thaw, so that udev can pick up the event and spawn
#a new xonard instance

case "$1" in
        hibernate)
		#Remove any stale temporary file
		rm -f -- /tmp/asus-xonar-u1-devices
		#Unbind Xonar U1 devices and store their IDs
		for dev in /sys/bus/usb/drivers/usbhid/[0-9]*
		do
			if [ -d $dev/0003:0B05:1743.* ]
			then
				devname=`basename $dev`;
				echo "$devname" >> /tmp/asus-xonar-u1-devices
				echo "$devname" > /sys/bus/usb/drivers/usbhid/unbind
			fi
		done
                ;;
        thaw)
		#Rebind stored Xonar U1 devices
		for devname in `cat /tmp/asus-xonar-u1-devices`
		do
			echo "$devname" > /sys/bus/usb/drivers/usbhid/bind
		done
		#Remove the temporary file
		rm -f -- /tmp/asus-xonar-u1-devices
                ;;
        *) exit $NA
                ;;
esac
