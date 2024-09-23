# A very dumb Makefile

all:	server client

server:
	gcc -Wall -o udp-server udpKnockKnockserver.c AddressUtility.c DieWithMessage.c

client:
	gcc -Wall -o udp-client udpKnockKnockclient.c AddressUtility.c DieWithMessage.c

clean:
	rm -f *.o udp-server udp-client

