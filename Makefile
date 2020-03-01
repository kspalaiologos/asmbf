
# This file could have been written in more
# elegant manner, but I'm fine with it's current
# state.

# Kamila Szewczyk, Jul 2019

export CFLAGS=-Ofast -march=native -funroll-loops -fomit-frame-pointer $(COVERAGE) $(OPTIONS)
TARGETS=bfasm bfi bfintd bconv bfstrip bfderle bflabels bfdata

.PHONY: all clean install uninstall test bfpp

all: $(TARGETS) bfpp bfasm.b bin

bfpp:
	make -C bfpp

install:
	chmod a+x bin/*
	sudo cp -rf bin/* /bin/

uninstall:
	cd /bin && sudo rm -f $(TARGETS) bfpp bfmake bfi-rle && cd -

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
	cp $(TARGETS) bfpp/bfpp bfmake bfi-rle bin/
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
