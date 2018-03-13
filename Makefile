BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man/man8
CC =		cc
CFLAGS =	-O -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long
LDFLAGS =
PORT= 8001

s: 
	gcc $(CFLAGS) $(OPTFLAGS) -w -o server my_server.c $(LDFLAGS)
	./server_udp

c: 
	gcc $(CFLAGS) $(OPTFLAGS) -w -o client my_client.c $(LDFLAGS)
	./client_udp

all:
	gcc $(CFLAGS) $(OPTFLAGS) -w -o server my_server.c $(LDFLAGS)
	gcc $(CFLAGS) $(OPTFLAGS) -w -o client my_client.c $(LDFLAGS)


clean:
	rm -f server client transmit.txt *.o core core.* *.core


