SRC=active_proxy
CC=gcc
LD=ld
CFLAGS=-Wall -g -std=gnu11  -c -fPIC -I/home/jtacquaviva/DDN/PROJET/EXCALIBUR/Excalibur-CS 
LDFLAGS=-shared

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

all: $(SRC).o
	$(LD) $(LDFLAGS) $^ -o $(SRC).so

clean:
	rm -f *.o *.so

