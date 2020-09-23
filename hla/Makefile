
CFLAGS=-g3 -Wall -Wextra -pedantic

.PHONY: clean all
all: hla

clean:
	rm -f *.o hla

lex.yy.c: lexer.l
	flex --full -o $@ $^

parser.tab.c: parser.y
	bison -d -v -Wcounterexamples -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

hla: parser.tab.o lex.yy.o node.o codegen.o main.o
	$(CC) $(CFLAGS) -o $@ $^
