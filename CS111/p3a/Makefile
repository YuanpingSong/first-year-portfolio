#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

CC=gcc
CFLAGS = -Wall -Wextra -O3

default:
	$(CC) $(CFLAGS) -lm -o lab3a lab3a.c

clean:
	-rm lab3a *.gz

dist:
	tar -czf lab3a-xxxxxxxxx.tar.gz Makefile README ext2_fs.h lab3a.c
