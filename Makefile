
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Krzysztof Szewczyk, Jul 2019

export CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer -w $(COVERAGE) $(OPTIONS)
TARGETS=bfasm bfvm bfi bconv bfstrip bfderle bflabels constpp bfdata effective vxcall

.PHONY: all clean setup test bfpp

all: $(TARGETS) bfpp etc/bfasm.b bin

setup:
	rm -rf "${HOME}/.asmbf/"
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
	rm -f effective.c constpp.c bflabels.c bfdata.c
	cd bfpp && make clean && cd ..

etc/bfasm.b: bfasm bfstrip etc/bfasm.asm
	./bfasm < etc/bfasm.asm | ./bfstrip > $@

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
	$(CC) $(CFLAGS) $^ -o $@

bflabels.c: bflabels.lex
	lex -f -o $@ $^

effective: effective.c
	$(CC) $(CFLAGS) $^ -o $@

effective.c: effective.lex
	lex -f -o $@ $^

constpp: constpp.c
	$(CC) $(CFLAGS) $^ -o $@

constpp.c: constpp.lex
	lex -f -o $@ $^

bfdata: bfdata.c
	$(CC) $(CFLAGS) $^ -o $@

bfdata.c: bfdata.lex
	lex -f -o $@ $^

vxcall: vxcall.c
	$(CC) $(CFLAGS) $^ -o $@

vxcall.c: vxcall.lex
	lex -f -o $@ $^
