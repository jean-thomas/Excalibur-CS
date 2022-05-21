CFLAGS= -Wall -g -std=c11 -frecord-gcc-switches `pkg-config fuse3 --cflags --libs`
OBJS=ioctl_client.o ioctl.o
CC=gcc
SHELL := /bin/bash


all: EXCBR_csfs EXCBR_csfs_client

EXCBR_csfs: EXCBR_CSFS.c EXCBR_CSFS_fnct.c
	$(CC) $^ -o $@ $(CFLAGS)

EXCBR_csfs_client: EXCBR_csfs_client.c EXCBR_CSFS_fnct.c
	$(CC) $^ -o $@ $(CFLAGS)

run:
	./EXCBR_csfs -d -o source=REPO TEST

test:
	./mini_test_cs.sh TEST

client: mini_cs
	echo "Hello World, this is a test"  > TEST/test3.txt
	./EXCBR_csfs_client TEST/test3.txt

free:
	fusermount -u -z TEST
clean:
	rm -f *.o EXCBR_csfs_client EXCBR_csfs
	fusermount -u -z TEST
