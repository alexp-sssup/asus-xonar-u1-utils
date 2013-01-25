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
 * \file xonard.h
 * \brief Xonard daemon
 */

#ifndef XONARD_H
#define XONARD_H

#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/hidraw.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>

#include "xonar.h"

#if ENABLE_CTL == 1
/**
 * \brief Initialize Inter-Process communication, create a unix socket.
 * \return A file descriptor to a socket on success or a negative number on
 * error.
 */
int initIPC();

/**
 * \brief Stop Inter-Process communication.
 * \param[in] sockfd A file descriptor to the unix socket.
 */
void destroyIPC(int sockfd);

/**
 * \brief Get message from the socket.
 * \param[in] sockfd A file descriptor to the unix socket.
 * \return The bytes received from the unix socket.
 */
char *getMsg(int sockfd);

/**
 * \brief Wait for an incoming message.
 * \param[in] sockfd A file descriptor to the unix socket.
 */
void waitMsg(int sockfd);

/**
 * \brief Wait for an event from both file descriptor.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] hidfd A file descriptor to the hidraw interface.
 * \return The file descriptor referring to the event.
 */
int waitForEvent(int sockfd, int hidfd);

/**
 * \brief Check if data is available on the socket.
 * \param[in] sockfd A file descriptor to the socket.
 */
int dataReady(int sockfd);

/**
 * \brief Process data packet received from the socket.
 * \param[in] hidfd A file descriptor to the hidraw interface.
 * \param[in] payload The bytes received from the socket.
 */
void processPacket(int hidfd, char *payload);

#endif

/**
 * \brief Change global configuration.
 * \param[in] hidfd File descriptor to the hidraw interface.
 * \param[in] conf Configuration bytes to send.
 * \return 0 on success.
 */
int sendGlobalConfPacket(int hidfd, uint8_t conf);

/**
 * \brief Change blinking configuration.
 * \param[in] hidfd File descriptor to the hidraw interface.
 * \param[in] ledIndex Can be ASUS_XONAR_U1_BLUE_LED or ASUS_XONAR_U1_RED_LED.
 * \param[in] dutyCycleA (in 20ms increment)Time spent OFF for the blue led or time spent ON for
 * the red led.
 * \param[in] dutyCycleTotal (in 20ms increment) Total time of a blinking cycle.
 * \return 0 on success.
 */
int sendBlinkConfPacket(int hidfd, uint8_t ledIndex, uint8_t dutyCycleA, uint8_t dutyCycleTotal);

void bailoutUinputConfig();

/**
 * \brief Send a keypress to an uinput file descriptor.
 * \param[in] uinputfd uinput file descriptor.
 * \param[in] keyCode Keycode to send.
 */
void sendKeyPress(int uinputfd, int keyCode);

/**
 * \brief Send a KEY_VOLUMEDOWN to uinput.
 * \param[in] uinputfd File descriptor to uinput.
 */
void handleVolumeDown(int uinputfd);

/**
 * \brief Send a KEY_VOLUMEUP to uinput.
 * \param[in] uinputfd File descriptor to uinput.
 */
void handleVolumeUp(int uinputfd);

/**
 * \brief Send a KEY_MUTE to uinput.
 * \param[in] uinputfd File descriptor to uinput.
 */
void handleMute(int uinputfd);

#endif

