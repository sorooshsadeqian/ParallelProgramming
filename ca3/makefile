CC = g++
CPPFLAGS = -std=c++11 -Wall -pedantic `pkg-config --cflags --libs opencv4` -msse4.1
.PHONY: all clean

all: clean p1 p2

p1: 1.cpp
	$(CC) 1.cpp -o p1 $(CPPFLAGS) 

p2: 2.cpp
	$(CC) 2.cpp -o p2 $(CPPFLAGS) 

clean:
	rm -f p1 p2
