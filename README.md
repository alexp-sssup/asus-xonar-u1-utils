# Xonar U1 audio controls

Xonard: a simple daemon to handle the non standard audio controls on Asus Xonar U1 devices
Copyright 2012-2015 Alessandro Pignotti

## Description

Asus Xonar U1 are pretty nice USB audio devices with a decent audio quality and a knob to control the volume and mute status of the device. The audio device itself is completely supported by linux using the snd-usb-audio kernel module. The control interface identifies itself as a standard HID devices, unfortunately this seems to be not actually true.

Xonard will configure the device to properly deliver interrupts when the knob is used using the hidraw kernel interface and deliver the events to the system using the uinput kernel interface. As an added bonus the blue LED blinking is set to something reasonable.

I've also reverse engineered how to configure the blinking of both the blue and red LEDs and the code should be pretty self explanatory about how to use this functionality. But this is not currently used beside setting a sane default

## Installation

- Clone the repo, compile and install:  
    `git clone git@github.com:alexp-sssup/asus-xonar-u1-utils.git ~/xonar/`  
    `cd ~/xonar/`  
    `make`  
    `sudo make install`
- Start the daemon:  
    `sudo xonard /dev/hidraw0`  
    (note it could be another number, check with `ls /dev/hidraw*`)
- Bind the keys:
    - Open your shortcuts (Ubuntu: System Settings -> Keyboard -> Shortcuts)
    - Add a shortcut (Ubuntu: click on the `+` button on bottom)
    - Add commands:  
        (note: it can be another card number, open `alsamixer` and press `F6` to find out)
        - Title: `Asus U1 volume up`  
            Command: `amixer -q -c 1 set PCM 2%+`  
            Shortcut: rotate the volume knob in the desired direction
        - Title: `Asus U1 volume down`  
            Command: `amixer -q -c 1 set PCM 2%-`  
            Shortcut: rotate the volume knob in the desired direction
        - By default, pressing the knob is recognized as system-wide `mute`

## Dirty sound fix

When plugging the card on many Linux distros (particularly Ubuntu) and listening through analog devices (like headphones), the sound is disturbed with noises and clicks. This is due to the optical output being on by default. To fix this:
- Visually inspect the headphones plug. If you see a red light, optical output is on.
- Open `alsamixer`, press `F6` and select the `U1` card
- Navigate with the right arrow to the `Digital` control
- Press `m` to toggle it mute (will switch from `00` to `MM`)
- The red light should be off now. Plug in the headphones and verify if the problem is solved.

## Enjoy your hardware

The Asus Xonar U1 is a great piece of hardware, which plays much better than you can imagine unless you have the proper setup. Try activating the optical output, getting a toslink optical cable (they're super cheap) and connect it to a good hi-fi (not so cheap) for maximum fidelity. Or just get some good headphones (cheaper than you think). Then get some high-quality music: if `MP3` you'll notice the difference between a low-quality and high-quality encoding (say between 128kBps and 320kBps). Or get a CD/DVD, or `flac` files, and search online for the keyword `audiophile`. Enjoy your hardware.

Also, spend some time playing around with `alsamixer`. If your headphones can sustain higher volume, unmute the `Loudness` control: it activates the internal amplifier, use it with caution, it delivers high volume without quality loss.

## Disclaimer

The code should be pretty safe, but consider that it does not come from official documentation, since I've found none. Don't blame me if it does harm your device or your cat: read the `LICENSE` before usage.