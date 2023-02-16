CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -gdwarf-4 -O3



SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o)

.PHONY: all clean spotless

all: banhammer

banhammer: $(OBJECTS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o

spotless:
	rm -f banhammer *.o

format:
	clang-format -i -style=file banhammer.c bf.c bv.c ht.c ll.c node.c parser.c