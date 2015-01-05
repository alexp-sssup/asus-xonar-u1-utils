# Xonar U1 audio controls

Xonard: a simple daemon to handle the non standard audio controls on Asus Xonar U1 devices
Copyright 2012-2015 Alessandro Pignotti

Edited 2015 Giuseppe Cuccu

## Description

Asus Xonar U1 are pretty nice USB audio devices with a decent audio quality and a knob to control the volume and mute status of the device. The audio device itself is completely supported by linux using the snd-usb-audio kernel module. The control interface identifies itself as a standard HID devices, unfortunately this seems to be not actually true.

Xonard will configure the device to properly deliver interrupts when the knob is used using the hidraw kernel interface and deliver the events to the system using the uinput kernel interface. As an added bonus the blue LED blinking is set to something reasonable.

I've also reverse engineered how to configure the blinking of both the blue and red LEDs and the code should be pretty self explanatory about how to use this functionality. But this is not currently used beside setting a sane default

## Installation

- Clone the repo, compile and install:  
    `git clone git@github.com:giuseppecuccu/asus-xonar-u1-utils.git ~/xonar/`  
    `cd ~/xonar/`  
    `make`  
    `sudo make install`
- Start the daemon:  
    `sudo xonard /dev/hidraw0`  

## Custom keybinding

Compiling with `make custom` will make the xonar use custom keybinding, returning keystrokes not available on a standard US keyboard. Such keystrokes can be customized for advanced control.
- Follow installation instructions, substituting `make custom` for your `make` call
- Bind the keys

In Ubuntu you can add shortcuts from `System Settings -> Keyboard -> Shortcuts`, click on the `+` button on the bottom.  
Here are some example bindings:
- Title: `Asus U1 volume up`  
    Command: `amixer -q -c 1 set PCM 1dB+`  
    Shortcut: rotate the volume knob in the desired direction
- Title: `Asus U1 volume down`  
    Command: `amixer -q -c 1 set PCM 1dB-`  
    Shortcut: rotate the volume knob in the desired direction
- Title: `Asus U1 toggle play/pause`  
    Command: `xdotool key XF86AudioPlay`  
    Shortcut: press the volume knob

## Notes
- The device number can differ, check with `ls /dev/hidraw*`.
- The card number can differ, open `alsamixer` and press `F6` to find out.
- To once shortcuts are set at OS level, you can switch between setups with the following:
    - `make; sudo make install; sudo xonard /dev/hidraw0`
    - `make custom; sudo make install; sudo xonard /dev/hidraw0`

## Dirty sound fix

When plugging the card on many Linux distros (particularly Ubuntu) and listening through analog devices (like headphones), the sound is disturbed with noises and clicks. This is due to the optical output being on by default. To fix this:
- Visually inspect the headphones plug. If you see a red light, optical output is on.
- Open `alsamixer`, press `F6` and select the `U1` card
- Navigate with the right arrow to the `Digital` control
- Press `m` to toggle it mute (will switch from `00` to `MM`)
- The red light should be off now. Plug in the headphones and verify if the problem is solved.

To make sure the problem does not represent itself every time you disconnect/reconnect the xonar, save the alsamixer settings with `sudo alsactl store 1`. If you still have problems you can check [here](http://ubuntuforums.org/showthread.php?t=1652691) for further assistance.

## Enjoy your hardware

The Asus Xonar U1 is a great piece of hardware, which plays much better than you can imagine unless you have the proper setup. Try activating the optical output, getting a toslink optical cable (they're super cheap) and connect it to a good hi-fi (not so cheap) for maximum fidelity. Or just get some good headphones (cheaper than you think). Then get some high-quality music: if `MP3` you'll notice the difference between a low-quality and high-quality encoding (say between 128kbps and 320kbps). Or get a CD/DVD, or `flac` files, and search online for the keyword `audiophile`. Enjoy your hardware.

Also, spend some time playing around with `alsamixer`. If your headphones can sustain higher volume, unmute the `Loudness` control: it activates the internal amplifier, use it with caution, it delivers high volume without quality loss.
You can bind a shortcut (like pressing the volume knob) to this command to toggle the amplifier on and off: `amixer -c 1 set Loudness toggle`.


## Disclaimer

The code should be pretty safe, but consider that it does not come from official documentation, since I've found none. Don't blame me if it does harm your device or your cat: read the `LICENSE` before usage.
