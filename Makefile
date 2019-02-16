CC=gcc
CFLAGS=-I.

build:
	$(CC) -O3 -s  -o bin/anagram main.c

clean:
	rm -rf bin/anagram

all: clean build
