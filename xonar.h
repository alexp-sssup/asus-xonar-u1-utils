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

/**
 * \file xonar.h
 * \brief Xonar global settings
 */

#ifndef XONAR_H
#define XONAR_H

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#ifndef CUSTOM_KEYBIND
//Default keybinding binds the xonar controls to the media keys
#define CLOCK_WISE KEY_VOLUMEUP
#define COUNTER_CLOCK_WISE KEY_VOLUMEDOWN
#define PUSH KEY_MUTE
#else
//Custom keybinding binds them to unused keys - here from Japanese keyboards
#define CLOCK_WISE KEY_KATAKANA
#define COUNTER_CLOCK_WISE KEY_HIRAGANA
#define PUSH KEY_KATAKANAHIRAGANA
#endif

//Change it to 0 to disable xonarctl and all user-level control.
#define ENABLE_CTL 1

#if ENABLE_CTL == 1

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/run/xonard.sock"
//Change the mask if you want to allow root only
#define SOCK_MODE 0666
#define MAX_FRAME_SIZE 9

//Message type
#define CONFIG_FRAME 0x01
#define BLINK_FRAME 0x02

#endif

#define ASUS_XONAR_VENDOR_ID 0x0b05
#define ASUS_XONAR_PRODUCT_ID 0x1743

//Setup of controls and leds
//	bit 7 -> Enable interrupt to report the control status
//	bit 6-5 -> Unknown/nothing
//	bit 4-3 -> 11: Blue LED blinks, 00/01/10: Blue LED always on
//	bit 2-1 -> 11: Red LED blinks always, 10: Red LED always off, 00/01: Red LED blinks on audio activity
//	bit 0 -> Unknown/nothing
#define ASUS_XONAR_U1_ENABLE_INTERRUPT (1<<7)
#define ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING (3<<3)
#define ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID (1<<3)
#define ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING (3<<1)
#define ASUS_XONAR_U1_ENABLE_RED_LED_AUDIO (1<<1)
#define ASUS_XONAR_U1_DISABLE_RED_LED (2<<1)

//ledIndex
#define ASUS_XONAR_U1_BLUE_LED 0x13
#define ASUS_XONAR_U1_RED_LED 0x11

#endif
