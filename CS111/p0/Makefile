#NAME: Yuanping Song
#EMAIL: yuanping.song@outlook.com
#ID: 

SHELL := /bin/bash
CC=gcc
CFLAGS =-Wall -Wextra

lab0:
	$(CC) $(CFLAGS) -o lab0 lab0.c 
check: lab0
# initialize test input
	@touch infile
# command line option test
	@echo "testing command line arguments"
	@echo abc | ./lab0 > /dev/null
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: ./lab0\n"; fi
	@./lab0 --input=infile > /dev/null
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: ./lab0 --input=infile > /dev/null\n"; fi
	@echo abc | ./lab0 --output=outfile 
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: echo abc | ./lab0 --output=outfile\n"; fi
	@echo abc | ./lab0 --catch > /dev/null
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: echo abc | ./lab0 --catch > /dev/null\n"; fi
	@./lab0 --input=infile --output=outfile --catch 
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: ./lab0 --input=infile --output=outfile --catch \n"; fi
# file redirection test
	@echo "testing file redirection"
	@head -c 1000 /dev/urandom > infile
	@./lab0 --input=infile --output=outfile 
	@cmp infile outfile
	@return_value=$?
	@if [[ $return_value -ne 0 ]]; then echo "test case failed: input is different from output"; fi
# clean up
	@rm infile outfile
	@echo "End of smoke test. Success if no failure messages!"

clean: lab0
	rm lab0 lab0-xxxxxxxxx.tar.gz
dist:
	tar -czf lab0-xxxxxxxxx.tar.gz lab0.c Makefile backtrace.png breakpoint.png README
