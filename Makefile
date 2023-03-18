CFLAGS= -Wall -g -std=gnu11 -frecord-gcc-switches `pkg-config fuse3 --cflags --libs`
OBJS=ioctl_client.o ioctl.o
CC=gcc
SHELL := /bin/bash


all: EXCBR_csfs csfs_application array_as_file

EXCBR_csfs: EXCBR_CSFS.c EXCBR_CSFS_fnct.o EXCBR_CS_exec.o
	$(CC) $^ -o $@ $(CFLAGS)

EXCBR_CSFS_fnct.o: EXCBR_CSFS_fnct.c EXCBR_CSFS_fnct.h EXCBR_common.h
	$(CC) -c EXCBR_CSFS_fnct.c -o $@ $(CFLAGS)

EXCBR_CS_exec.o: EXCBR_CS_exec.c EXCBR_CS_exec.h
	$(CC) -c EXCBR_CS_exec.c -o $@ $(CFLAGS)

app_using_cs.o: app_using_cs.c EXCBR_CSFS_fnct.o
	$(CC) -c app_using_cs.c -o $@ $(CFLAGS)

csfs_application: EXCBR_CSFS_fnct.o app_using_cs.o
	$(CC)  app_using_cs.o EXCBR_CSFS_fnct.o -o $@ $(CFLAGS)

array_as_file: array_as_file.c
	$(CC) $^ -o $@ $(CFLAGS)

run:
	./EXCBR_csfs -d -o source=REPO TEST

test:
	./minitest_EXCBR_csfs.sh TEST

free:
	fusermount -u -z TEST
clean:
	rm -f *.o array_as_file EXCBR_csfs csfs_application
	fusermount -u -z TEST
