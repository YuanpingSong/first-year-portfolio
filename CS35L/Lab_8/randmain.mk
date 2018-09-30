OPTIMIZE = -O2

CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -march=native -mtune=native -mrdrnd


randmain: 
	# statically link randcpuid object code. 
	$(CC) $(CFLAGS) -c randcpuid.c -o randcpuid.o
	# statically link randmain object code. 
	$(CC) $(CFLAGS) -c randmain.c -o randmain
	# enable bactrace 
	$(CC) $(CFLAGS) -ldl -Wl, -rpath=${PWD}

randlibhw.so:
	# create object file for randlibhw
	$(CC) $(CFLAGS) -fPIC, -c randlibhw.c -o randlibhw.o
	# create shared library
	$(CC) $(CFLAGS) -shared -o randlibhw.so randlibhw.o

randlibsw.so: 
	# create object file for randlibsw
	$(CC) $(CFLAGS) -fPIC, -c randlibsw.c -o randlibsw.o
	# create shared library
	$(CC) $(CFLAGS) -shared -o randlibsw.so randlibsw.o