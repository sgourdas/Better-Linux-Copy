HEADER  = Types.h
SOURCE	= quic.c util.c iList.c dirList.c
OBJS 	= quic.o util.o iList.o dirList.o
EXEC   = quic
CC	    = gcc
CFLAGS  = -g -Wall -Werror


make: quic.o util.o iList.o dirList.o
	$(CC) $(CFLAGS) quic.o util.o iList.o dirList.o -o quic

quic.o: quic.c
	$(CC) -c $(CFLAGS) quic.c

util.o: util.c
	$(CC) -c $(CFLAGS) util.c

iList.o: iList.c
	$(CC) -c $(CFLAGS) iList.c

dirList.o: dirList.c
	$(CC) -c $(CFLAGS) dirList.c

clean:
	rm -f $(OBJS) $(EXEC)