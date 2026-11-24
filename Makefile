CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: create remove

create: create.c
	$(CC) $(CFLAGS) -o $@ $<

remove: remove.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f create remove
	rm -rf Lab1_Erkinov

.PHONY: all clean
