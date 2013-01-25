all: xonard xonarctl

xonard: xonard.c
	gcc -Wall -o $@ $^

xonarctl: xonarctl.c
	gcc -Wall -o $@ $^

install: xonard xonarctl
	cp -a 16-asus-xonar-u1.rules /etc/udev/rules.d/
	cp -a 16-asus-xonar-u1.sh /etc/pm/sleep.d/
	cp -a xonard /usr/local/bin/
	cp -a xonarctl /usr/local/bin/

uninstall:
	rm -rf /etc/udev/rules.d/16-asus-xonar-u1.rules
	rm -rf /etc/pm/sleep.d/16-asus-xonar-u1.sh
	rm -rf /usr/local/bin/xonard
	rm -rf /usr/local/bin/xonarctl

clean:
	rm -rf *.o

mrproper: clean
	rm -f xonard
	rm -f xonarctl
