all: xonard

xonard: xonard.cpp
	g++ -o $@ $^
