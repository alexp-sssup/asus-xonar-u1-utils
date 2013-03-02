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

#include "xonarctl.h"

#if ENABLE_CTL == 1

void sendCustom(int sockfd, char type, char payload[]) {
    char fullPayload[9];
    if (type == CONFIG_FRAME) {
        fullPayload[0] = 0x01;
        strncpy(fullPayload+1, payload, 8);
        sendMsg(sockfd, fullPayload, 9);
    }
    else if (type == BLINK_FRAME) {
        fullPayload[0] = 0x02;
        strncpy(fullPayload+1, payload, 6);
        sendMsg(sockfd, fullPayload, 7);
    }
}

void sendBlink(int sockfd, short ledIndex, short timeOn, short period) {
    char payload[7];
    sprintf(payload, "%02x", ledIndex);
    sprintf(payload+2, "%02x", timeOn);
    sprintf(payload+4, "%02x", period);
    sendCustom(sockfd, BLINK_FRAME, payload);
}

void sendConf(int sockfd, int conf) {
    char payload[9];
    sprintf(payload, "%08x", conf);
    sendCustom(sockfd, CONFIG_FRAME, payload);
}

void setBlueStill(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID |
            ASUS_XONAR_U1_DISABLE_RED_LED);
}

void setBlueBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_DISABLE_RED_LED);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, timeOn, period);
}

void setBlueRedBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, 101, 100);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, timeOn, period);
}

void setRedStill(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, 101, 100);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, 0, 0);
}

void setRedBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, timeOn, period);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, 0, 0);
}

void setRedBlueBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, timeOn, period);
}

void setRedAudio(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_ENABLE_RED_LED_AUDIO);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, 0, 0);
}

void setPurpleStill(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, 201, 200);
}

void setPurpleBlink(int sockfd, int timeOn, int period) {
    printf("Currently unavailable, because it's glitchy.\n");
}

void setPurpleBlueBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, timeOn, period);
}

void setPurpleRedBlink(int sockfd, int timeOn, int period) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_ENABLE_RED_LED_BLINKING);
    sendBlink(sockfd, ASUS_XONAR_U1_RED_LED, 201, 200);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, timeOn+1, period);
}

void setPurpleAudio(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_SOLID |
            ASUS_XONAR_U1_ENABLE_RED_LED_AUDIO);
}

void setDisabled(int sockfd) {
    sendConf(sockfd,
            ASUS_XONAR_U1_ENABLE_INTERRUPT |
            ASUS_XONAR_U1_ENABLE_BLUE_LED_BLINKING |
            ASUS_XONAR_U1_DISABLE_RED_LED);
    sendBlink(sockfd, ASUS_XONAR_U1_BLUE_LED, 0, 0);
}

void setVolumeDependent(int sockfd, short volume) {
    if (volume < 0)
        volume = 0;
    else if (volume > 100)
        volume = 100;
    int dutyCycle;
    if (volume < 51) {
        dutyCycle = volume*2;
        setPurpleBlueBlink(sockfd, dutyCycle, 100);
    }
    else {
        volume -= 50;
        dutyCycle = volume*1.8;
        setPurpleRedBlink(sockfd, dutyCycle, 100);
    }
}

int initIPC()
{
    int sockfd;
    struct sockaddr_un sock;
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Unable to create socket");
        return -1;
    }
    memset(&sock, 0, sizeof(struct sockaddr_un));
    sock.sun_family = AF_UNIX;
    snprintf(sock.sun_path, sizeof(sock.sun_path), SOCK_PATH);
    if (connect(sockfd, (struct sockaddr*)&sock, sizeof(struct sockaddr_un)) != 0) {
        perror("Unable to connect to socket");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

void destroyIPC(int sockfd)
{
    if (sockfd > 0) {
        close(sockfd);
    }
}

void sendMsg(int sockfd, char *buffer, char frameSize)
{
    char sendBuffer[frameSize+1];
    sendBuffer[0] = frameSize;
    strncpy(sendBuffer+1, buffer, frameSize);
    write(sockfd, sendBuffer, frameSize+1);
}

void printUsage(char *appName) {
    printf("Usage: %s [options]\n", appName);
    printf("OPTIONS:\n");
    printf("  -h, --help\t\t\tPrint this message.\n");
    printf("  -b, --blue\t\t\tSet color to blue.\n");
    printf("  -r, --red\t\t\tSet color to red.\n");
    printf("  -p, --purple\t\t\tSet color to purple.\n");
    printf("      --blue-red\t\tSet blinking blue to red.\n");
    printf("      --red-blue\t\tSet blinking from red to blue.\n");
    printf("      --purple-blue\t\tSet color from purple to blue.\n");
    printf("      --purple-red\t\tSet color from purple to red.\n");
    printf("      --disable\t\t\tDisable the LEDs.\n");
    printf("  -s, --still\t\t\tMake the LED still.\n");
    printf("  -a, --audio\t\t\tMake the red LED react to audio.\n");
    printf("  -v, --volume\t\t\tMake the red LED behaviour volume dependent.\n");
    printf("  -d, --duty-cycle <value>\tSpecify time spent on vs time spent off in %%.\n");
    printf("      --period <value>\tSpecify time spent off vs time spent on in ms (max 255).\n");
}

int main(int argc, char *argv[])
{
    int ledType = 0;
    short audioBlink = 0;
    short dutyCycle = 50;
    short volume = 0;
    //FIXME Period different than 200 produces flickering.
    short period = 200;
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    int optsIndex = 0;
    struct option longOptions[] = {
        {"blue", no_argument, NULL, 1},
        {"red", no_argument, NULL, 2},
        {"purple", no_argument, NULL, 3},
        {"blue-red", no_argument, NULL, 4},
        {"red-blue", no_argument, NULL, 5},
        {"purple-blue", no_argument, NULL, 6},
        {"purple-red", no_argument, NULL, 7},
        {"disable", no_argument, NULL, 8},
        {"still", no_argument, NULL, 9},
        {"audio", no_argument, NULL, 10},
        {"volume", required_argument, NULL, 11},
        {"duty-cycle", required_argument, NULL, 12},
        {"period", required_argument, NULL, 13},
        {"help", no_argument, NULL, 99}
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "brpsahv:d:", longOptions, &optsIndex)) != -1) {
        if (opt > 0 && opt < 8) {
            if (ledType != 0) {
                printf("Too much arguments!\n");
                printUsage(argv[0]);
                return 1;
            }
            ledType = opt;
        }
        else {
            switch (opt) {
                case 'b':
                    ledType = 1;
                    break;
                case 'r':
                    ledType = 2;
                    break;
                case 'p':
                    ledType = 3;
                    break;
                case 8:
                    ledType = 8;
                    break;
                case 9:
                case 's':
                    dutyCycle = 0;
                    break;
                case 10:
                case 'a':
                    audioBlink = 1;
                    break;
                case 11:
                case 'v':
                    volume = atoi(optarg);
                    break;
                case 12:
                case 'd':
                    dutyCycle = atoi(optarg);
                    if (dutyCycle > 100) {
                        printf("Duty cycle value too high!\n");
                        return 1;
                    }
                    else if (dutyCycle < 0) {
                        printf("Duty cycle value too low!\n");
                        return 1;
                    }
                    break;
                case 13:
                    period = atoi(optarg);
                    if (period > 255) {
                        printf("Period value too high!\n");
                        return 1;
                    }
                    else if (period < 0) {
                        printf("Period value too low!\n");
                        return 1;
                    }
                    break;
                default:
                    printUsage(argv[0]);
                    return 1;
            }
        }
    }
    if (ledType == 0 && dutyCycle && !volume) {
        printUsage(argv[0]);
        return 1;
    }
    int sockfd = initIPC();
    if (sockfd < 0) {
        return 2;
    }
    if (volume) {
        setVolumeDependent(sockfd, volume);
    }
    else if (!audioBlink && dutyCycle > 0) {
        switch (ledType) {
            case 1:
                setBlueBlink(sockfd, 2*dutyCycle, period);
                break;
            case 2:
                setRedBlink(sockfd, 2*dutyCycle, period);
                break;
            case 3:
                setPurpleBlink(sockfd, 2*dutyCycle, period);
                break;
            case 4:
                setBlueRedBlink(sockfd, 2*dutyCycle, period);
                break;
            case 5:
                setRedBlueBlink(sockfd, 2*dutyCycle, period);
                break;
            case 6:
                setPurpleBlueBlink(sockfd, 2*dutyCycle, period);
                break;
            case 7:
                setPurpleRedBlink(sockfd, 2*dutyCycle, period);
                break;
            case 8:
                setDisabled(sockfd);
                break;
        }
    }
    else if (!audioBlink && dutyCycle == 0) {
        switch (ledType) {
            case 1:
                setBlueStill(sockfd);
                break;
            case 2:
                setRedStill(sockfd);
                break;
            case 3:
                setPurpleStill(sockfd);
                break;
        }
    }
    else {
        switch (ledType) {
            case 2:
                setRedAudio(sockfd);
                break;
            case 3:
                setPurpleAudio(sockfd);
                break;
        }
    }
    destroyIPC(sockfd);
    return 0;
}

#endif
