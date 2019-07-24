CC = gcc
CFLAGS= -std=c99 -Wall -Wextra

all: traceroute.o sender.o receiver.o
	$(CC) $(CFLAGS) traceroute.o sender.o receiver.o -o traceroute


traceroute.o: traceroute.c traceroute.h

sender.o: sender.c traceroute.h

receiver.o: receiver.c traceroute.h

clean:
	rm -f *.o

distclean:
	rm -f *o traceroute
