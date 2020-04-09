
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Kamila Szewczyk, Jul 2019

ifndef ASMBF_DIR
export ASMBF_DIR=/bin
endif

export ASMBF_DIR:=$(ASMBF_DIR)/asmbf/

export CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer $(COVERAGE) $(OPTIONS)
TARGETS=bfasm bfi bfintd bconv bfstrip bfderle bflabels bfdata

.PHONY: all clean install uninstall test bfpp

all: $(TARGETS) bfpp bfasm.b bin

bfpp:
	make -C bfpp

install:
	chmod a+x -R bin/*
	sudo cp -rf bin/* $(ASMBF_DIR)
	echo "$(ASMBF_DIR)" > ~/.asmbf

uninstall:
	sudo rm -rf $(ASMBF_DIR)

test: test/*.asm
	chmod a+x test.pl $^
	-bfi --help 2> /dev/null
	-bfi --version 2> /dev/null
	-bfi --blah 2> /dev/null
	./test.pl $^

clean:
	rm -rf bin/
	cd bfpp && make clean && cd ..

bfasm.b: bfasm bfasm.asm
	./bfasm < bfasm.asm > $@

bin: $(TARGETS)
	mkdir -p bin
	mkdir -p bin/asmbf
	mkdir -p bin/asmbf/lib
	cp $(TARGETS) bfpp/bfpp bfmake bfi-rle bin/asmbf/
	cp lib-bfm.lua bin/asmbf/lib/
	rm -rf $(TARGETS)
	
test-clean:
	rm -f test/*.aout
	rm -f test/*.b

bflabels: bflabels.c
	$(CC) $(CFLAGS) $^ -lfl -o $@

bflabels.c: bflabels.lex
	lex -o $@ $^

bfdata: bfdata.c
	$(CC) $(CFLAGS) $^ -lfl -o $@

bfdata.c: bfdata.lex
	lex -o $@ $^
