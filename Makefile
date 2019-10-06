
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Krzysztof Szewczyk, Jul 2019

CC=gcc
CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer -w
TARGETS=bfasm bfi bfintd bconv 

.PHONY: all clean

all: $(TARGETS) bfasm.b bin

clean:
	rm -rf bin/

bfasm.b: bfasm bfasm.asm
	./bfasm < bfasm.asm > $@

bin: $(TARGETS)
	mkdir -p bin
	cp $(TARGETS) bfpp bin/
	rm -rf $(TARGETS)
	
	