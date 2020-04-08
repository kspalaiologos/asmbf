
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Krzysztof Szewczyk, Jul 2019

export CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer $(COVERAGE) $(OPTIONS)
TARGETS=bfasm bfi bfintd bconv bfstrip bfderle bflabels bfdata

.PHONY: all clean install uninstall test bfpp

all: $(TARGETS) bfpp bfasm.b bin

bfpp:
	make -C bfpp

install:
	chmod a+x -R bin/*
	sudo cp -rf bin/* /bin/asmbf/

uninstall:
	sudo rm -rf /bin/asmbf

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
	cp $(TARGETS) bfpp/bfpp lib-bfm.lua bfmake bfi-rle bin/asmbf/
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
