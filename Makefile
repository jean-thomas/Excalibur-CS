CFLAGS= -Wall -g -std=gnu11 -frecord-gcc-switches `pkg-config fuse3 --cflags --libs`
OBJS=ioctl_client.o ioctl.o
CC=gcc
SHELL := /bin/bash


all: EXCBR_csfs csfs_application array_as_file

EXCBR_csfs: EXCBR_CSFS.c EXCBR_CSFS_fnct.c
	$(CC) $^ -o $@ $(CFLAGS)

csfs_application: app_using_cs.c
	$(CC) $^ -o $@ $(CFLAGS)

array_as_file: array_as_file.c
	$(CC) $^ -o $@ $(CFLAGS)

run:
	./EXCBR_csfs -d -o source=REPO TEST

test:
	./mini_test_cs.sh TEST

free:
	fusermount -u -z TEST
clean:
	rm -f *.o array_as_file EXCBR_csfs csfs_application
	fusermount -u -z TEST
