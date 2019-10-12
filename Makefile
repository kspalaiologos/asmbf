
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Kamila Szewczyk, Jul 2019

CC=gcc
CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer -w $(COVERAGE)
TARGETS=bfasm bfi bfintd bconv

.PHONY: all clean install uninstall test

all: $(TARGETS) bfasm.b bin

install:
	sudo cp -rf bin/* /bin/

uninstall:
	cd /bin && sudo rm -f $(TARGETS) bfpp bfmake strip.pl labels.pl && cd -

test: test/*.asm
	chmod a+x test.pl
	./test.pl $^

clean:
	rm -rf bin/

bfasm.b: bfasm bfasm.asm
	./bfasm < bfasm.asm > $@

bin: $(TARGETS)
	mkdir -p bin
	cp $(TARGETS) bfpp bfmake strip.pl labels.pl bin/
	rm -rf $(TARGETS)
	
	
