
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Krzysztof Szewczyk, Jul 2019

export CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer -w $(COVERAGE) $(OPTIONS)
TARGETS=bfasm bfi bfintd bconv bfstrip bfderle bflabels bfdata

.PHONY: all clean setup test bfpp

all: $(TARGETS) bfpp etc/bfasm.b bin

setup:
	mkdir -p "${HOME}/.asmbf/"
	cp -rf bin/asmbf/* "${HOME}/.asmbf/"
	chmod -R a+x "${HOME}/.asmbf/"
	echo "asm2bf has been installed to ${HOME}/.asmbf/."

bfpp:
	make -C bfpp

test: test/*.asm
	chmod a+x test/test.pl $^
	-bfi --help 2> /dev/null
	-bfi --version 2> /dev/null
	-bfi --blah 2> /dev/null
	test/test.pl $^

clean:
	rm -rf bin/
	cd bfpp && make clean && cd ..

etc/bfasm.b: bfasm etc/bfasm.asm
	./bfasm < etc/bfasm.asm > $@

bin: $(TARGETS)
	mkdir -p bin
	mkdir -p bin/asmbf
	mkdir -p bin/asmbf/lib
	cp $(TARGETS) bfpp/bfpp bfmake bfi-rle bin/asmbf/
	cp lib-bfm.lua bin/asmbf/lib/
	cp ide/src/bfstudio.py bin/asmbf/bfstudio
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
