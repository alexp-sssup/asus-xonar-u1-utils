.PHONY: all custom install uninstall clean mrproper

# Default keybinds are to media keys.
all: mrproper xonard xonarctl

# Calling "make custom" will make the xonar knob send special keystrokes.
# See the README about how to bind them to custom controls.
custom: mrproper xonard-custom xonarctl
	mv xonard-custom xonard

xonard-custom-opts = -D CUSTOM_KEYBIND=1

xonard xonard-custom: xonard.c
	gcc -Wall $($@-opts) -o $@ $^

xonarctl: xonarctl.c
	gcc -Wall -o $@ $^

install: xonard xonarctl uninstall
	cp -a 16-asus-xonar-u1.rules /etc/udev/rules.d/
	cp -a 16-asus-xonar-u1.sh /etc/pm/sleep.d/
	cp -a xonard /usr/local/bin/
	cp -a xonarctl /usr/local/bin/

uninstall:
	rm -rf /etc/udev/rules.d/16-asus-xonar-u1.rules
	rm -rf /etc/pm/sleep.d/16-asus-xonar-u1.sh
	killall -q xonard
	rm -rf /usr/local/bin/xonard
	rm -rf /usr/local/bin/xonarctl

clean:
	rm -rf *.o

mrproper: clean
	rm -f xonard
	rm -f xonarctl
