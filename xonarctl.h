/*
 * Xonarctl
 *
 * Copyright 2013 Stephane Albert
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
 * \file xonarctl.h
 * \brief Xonard control utility
 */

#ifndef XONARCTL_H
#define XONARCTL_H

#include <getopt.h>
#include <stdlib.h>
#include "xonar.h"

#if ENABLE_CTL == 1
/**
 * \brief Send a custom message over the socket.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] type The type of message to send.
 * \param[in] payload The bytes of configuration to send.
 */
void sendCustom(int sockfd, char type, char payload[]);

/**
 * \brief Send a blinking configuration message over the socket.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] ledIndex The led to modify.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void sendBlink(int sockfd, short ledIndex, short timeOn, short period);

/**
 * \brief Send a global configuration message over the socket.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] conf Configuration bytes to send.
 */
void sendConf(int sockfd, int conf);

/**
 * \brief Set the LED to blue without blinking.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setBlueStill(int sockfd);

/**
 * \brief Set the LED to blinking blue.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setBlueBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to blinking from blue to red.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setBlueRedBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to red without blinking.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setRedStill(int sockfd);

/**
 * \brief Set the LED to blinking red.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setRedBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to blinking from red to blue.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setRedBlueBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the red LED to react to audio.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setRedAudio(int sockfd);

/**
 * \brief Set the LED to purple without blinking.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setPurpleStill(int sockfd);

/**
 * \brief Set the LED to blinking purple.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setPurpleBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to blinking from purple to blue.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setPurpleBlueBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to blinking from blue to purple.
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] timeOn The LED on duration (in ms).
 * \param[in] period The total time of the cycle.
 */
void setPurpleRedBlink(int sockfd, int timeOn, int period);

/**
 * \brief Set the LED to purple and audio blinking.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setPurpleAudio(int sockfd);

/**
 * \brief Disable the LED output.
 * \param[in] sockfd A file descriptor to the socket.
 */
void setDisabled(int sockfd);

/**
 * \brief Change the LED output depending on the volume (from blue to red).
 * \param[in] sockfd A file descriptor to the socket.
 * \param[in] volume The current volume level in percent from 0 to 100.
 */
void setVolumeDependent(int sockfd, short volume);

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
 * \brief Send a message over the socket.
 * \param[in] sockfd A file descriptor to the unix socket.
 * \param[in] buffer Bytes to send on the unix socket
 * \param[in] frameSize Number of bytes to send on the unix socket
 */
void sendMsg(int sockfd, char *buffer, char frameSize);

/**
 * \brief Show help.
 * \param[in] Should be set to argv[0] to pring the application name.
 */
void printUsage(char *appName);

#endif
#endif
