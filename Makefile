CC=g++
CFLAGS=-ansi -Wall -pedantic-errors -O0 -std=c++11

all: bgame

dev: test/driver.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp
	$(CC)	$(CFLAGS)	test/driver.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp	message.c	logging.c	-o	test/driver
bgame: bgame.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp
	$(CC)	$(CFLAGS)	bgame.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp	message.c	logging.c	-o	bgame

clean:
	rm -f test/driver
	rm -f bgame
