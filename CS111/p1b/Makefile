#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

CC=gcc
CFLAGS =-Wall -Wextra -std=c11 -O3

lab1b: 
	$(CC) $(CFLAGS) -lz -o lab1b-client lab1b-client.c
	$(CC) $(CFLAGS) -lz -o lab1b-server lab1b-server.c
clean: lab1b
	rm lab1b-client lab1b-server lab1b-xxxxxxxxx.tar.gz
dist:
	tar -czf lab1b-xxxxxxxxx.tar.gz Makefile README lab1b-server.c lab1b-client.c
