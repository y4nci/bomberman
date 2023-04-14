CC=g++
CFLAGS=-ansi -Wall -pedantic-errors -O0

all: bgame

dev:
	$(CC)	$(CFLAGS)	test/driver.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp	-o	test/driver
bgame:
	$(CC)	$(CFLAGS)	bgame.cpp	lib/Bomb.cpp	lib/Bomber.cpp	lib/Map.cpp	lib/Obstacle.cpp	lib/Inputs.cpp	-o	bgame

clean:
	rm -f test/driver
	rm -f bgame
