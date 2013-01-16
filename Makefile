all: xonard

xonard: xonard.c
	gcc -Wall -o $@ $^

install: xonard
	cp -a 16-asus-xonar-u1.rules /etc/udev/rules.d/
	cp -a 16-asus-xonar-u1.sh /etc/pm/sleep.d/
	cp -a xonard /usr/local/bin/
