# Portable build: override the compiler with e.g. `make CC=clang`,
# `make CC=musl-gcc`, or `make CC=aarch64-linux-gnu-gcc`.
CC      ?= cc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2
LDLIBS   = -ldl

all: elfLoader hello.so

elfLoader: elfLoad.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $<

test: all
	./test.sh

clean:
	rm -f elfLoader *.so

.PHONY: all test clean
