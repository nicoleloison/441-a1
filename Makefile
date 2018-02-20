BINDIR =	/usr/local/sbin
MANDIR =	/usr/local/man/man8
CC =		cc
CFLAGS =	-O -ansi -pedantic -U__STRICT_ANSI__ -Wall -Wpointer-arith -Wshadow -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wno-long-long
LDFLAGS =
PORT= 8001

s: 
	gcc $(CFLAGS) $(OPTFLAGS) -o server_udp server_udp.c $(LDFLAGS)
	./server_udp

c: 
	gcc $(CFLAGS) $(OPTFLAGS) -o client_udp client_udp.c $(LDFLAGS)
	./client_udp

all:
	gcc $(CFLAGS) $(OPTFLAGS) -o cli cli.c $(LDFLAGS)
	gcc $(CFLAGS) $(OPTFLAGS) -o serv serv.c $(LDFLAGS)

clean:
	rm -f server_udp client_udp cli serv *.o core core.* *.core


