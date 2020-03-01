

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

.PHONY: all clean
all: bfpp

clean:
	rm -f $(OBJS) bfpp

bfpp: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm
