
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Kamila Szewczyk, Jul 2019

export CFLAGS=$(OPTIONS)
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
	rm -f preprocessor/*.c
	cd bfpp && make clean && cd ..

etc/bfasm.b: bfasm bfstrip etc/bfasm.asm
	./bfasm < etc/bfasm.asm | ./bfstrip > $@

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

bflabels: preprocessor/bflabels.lex
	lex -f -o preprocessor/bflabels.c $^
	$(CC) $(CFLAGS) preprocessor/bflabels.c -o $@

effective: preprocessor/effective.lex
	lex -f -o preprocessor/effective.c $^
	$(CC) $(CFLAGS) preprocessor/effective.c -o $@

constpp: preprocessor/constpp.lex
	lex -f -o preprocessor/constpp.c $^
	$(CC) $(CFLAGS) preprocessor/constpp.c -o $@

bfdata: preprocessor/bfdata.lex
	lex -f -o preprocessor/bfdata.c $^
	$(CC) $(CFLAGS) preprocessor/bfdata.c -o $@

vxcall: preprocessor/vxcall.lex
	lex -f -o preprocessor/vxcall.c $^
	$(CC) $(CFLAGS) preprocessor/vxcall.c -o $@
