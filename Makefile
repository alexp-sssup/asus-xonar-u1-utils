xonar_u1_setup: xonar_u1_setup.cpp
	g++ -o $@ $^ `pkg-config --cflags --libs libusb-1.0`
