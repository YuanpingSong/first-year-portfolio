#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

CC=gcc
CFLAGS = -Wall -Wextra -O3

default:
	$(CC) $(CFLAGS) -pthread -lmraa -lm -o lab4c_tcp lab4c_tcp.c
	$(CC) $(CFLAGS) -pthread -lmraa -lm -lssl -lcrypto -o lab4c_tls lab4c_tls.c

clean:
	-rm lab4c_tcp lab4c_tls *.gz

dist:
	tar -czf lab4c-xxxxxxxxx.tar.gz Makefile README lab4c_tcp.c lab4c_tls.c
