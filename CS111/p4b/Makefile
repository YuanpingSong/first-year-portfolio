#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

CC=gcc
CFLAGS = -Wall -Wextra -O3

default:
	$(CC) $(CFLAGS) -pthread -lmraa -lm -o lab4b lab4b.c

check: default
	echo -e "START\nSTOP\nOFF\n" | ./lab4b --period=3 --log=test.log --scale=F
	if [ $? -eq 0 ]; then echo "Success!"; else echo "Failure!"; fi

clean:
	-rm lab4b *.gz
dist:
	tar -czf lab4b-xxxxxxxxx.tar.gz Makefile README lab4b.c
