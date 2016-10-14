CC = gcc
CFLAGS = -O2 -Wall
LDFLAGS = -lcurl -lm

PREFIX = /usr

all: bingit

bingit: src/*.c
	$(CC) $(CFLAGS) -o bingit src/*.c $(LDFLAGS)

install: all
	@echo installing executables to ${PREFIX}/bin
	mkdir -p ${PREFIX}/bin
	cp -f bingit ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/bingit

clean:
	rm bingit

.PHONY: all bingit install clean
