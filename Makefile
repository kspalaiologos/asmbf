
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Kamila Szewczyk, Jul 2019

CC=gcc
CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer

.PHONY: all clean

all: bfasm-experimental bfasm bfasm.b bfi bfintd bfpp gisa

clean:
	rm -rf bfasm bfasm-experimental bfi bfpp bfintd gisa bfasm.b

bfasm-experimental: bfasm-experimental.c
	$(CC) $(CFLAGS) -o $@ $^

bfasm: bfasm.c
	$(CC) $(CFLAGS) -o $@ $^

bfasm.b: bfasm bfasm.asm
	./bfasm < bfasm.asm > $@

bfi: bfi.c
	$(CC) $(CFLAGS) -o $@ $^

bfintd: bfintd.c
	$(CC) $(CFLAGS) -o $@ $^

bfpp: bfpp.c
	$(CC) $(CFLAGS) -o $@ $^

gisa: gisa.c
	$(CC) $(CFLAGS) -o $@ $^
