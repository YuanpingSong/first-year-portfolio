#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

CC=gcc
CFLAGS = -Wall -Wextra -O3

.ONESHELL:

default:
	$(CC) $(CFLAGS) -g -pthread -lrt -lprofiler -o lab2_list lab2_list.c SortedList.c

profile:
	$(CC) $(CFLAGS) -g -pthread -lrt -lprofiler -o lab2_list lab2_list.c SortedList.c
	export CPUPROFILE_FREQUENCY=1000
	./lab2_list --threads=12 --iterations=1000 --sync=s --profile

test_init:
	echo "" > lab2b_list.csv;

test1: default
#Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
#Spin-lock synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv;
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv;

test2: default
#Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,16,24 threads
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv;
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv;

test3: default
#Run your program with --yield=id, 4 lists, 1,4,8,12,16 threads, and 1, 2, 4, 8, 16 iterations (and no synchronization) to see how many iterations it takes to reliably fail (and make sure your Makefile expects some of these tests to fail).
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2b_list.csv;
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv;
#Run your program with --yield=id, 4 lists, 1,4,8,12,16 threads, and 10, 20, 40, 80 iterations, --sync=s and --sync=m to confirm that updates are now properly protected (i.e., all runs succeeded).
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=10 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=20 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=40 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=80 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=10 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=20 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=40 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=4 --iterations=80 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=10 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=20 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=40 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=8 --iterations=80 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=s >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=m >> lab2b_list.csv;
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=m >> lab2b_list.csv;

test4: default
# Rerun both synchronized versions, without yields, for 1000 iterations, 1,2,4,8,12 threads, and 1,4,8,16 lists.
	./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=1 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=4 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=8 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=16 --sync=m >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=1 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=4 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=8 --sync=s >> lab2b_list.csv;
	./lab2_list --iterations=1000 --threads=16 --lists=16 --sync=s >> lab2b_list.csv;


tests: test_init test1 test2 test3 test4

graphs: tests
	gnuplot graph1.gp
	gnuplot graph2.gp
	gnuplot graph3.gp
	gnuplot graph4.gp
	gnuplot graph5.gp
clean: 
	-rm  lab2_list *.tar.gz
dist: graphs
	tar -czf lab2b-xxxxxxxxx.tar.gz Makefile README lab2_list.c SortedList.* *.csv *.png *.gp profile.out
