

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

CFLAGS:=-DLUA_USE_POPEN

.PHONY: all clean
all: bfpp

clean:
	rm -f $(OBJS) bfpp

bfpp: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm
