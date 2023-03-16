/*
  Excalibur Computation Storage:

  simple client to test the ioctl in the mini CSFS

*/

#include <linux/limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

#include "EXCBR_common.h"

void print_help(char * prog)
{
    printf ("Usage: %s [option] <file> \n", prog);
    fprintf (stderr,"Options:\n");
    fprintf (stderr,"-v count the number of vowels in a text file\n");
    fprintf (stderr,"-c cout the number of consonant in a text file\n");
    fprintf (stderr,"-a <type> compute the average value of data file of type int or double (default)\n");
    fprintf (stderr,"-h help, display this help message:\n");

    cs_help_fnct();
    return;
}
int main(int argc, char **argv)
{
    int fd;
    int nb_byte = -1;
    int ret = 0;
    char filename[PATH_MAX];
    struct stat file_stat;
    char *buf;
    struct cs_args_t cs_compad;
    size_t func;
    CS_TYPE type;
    int opt;

    while ((opt = getopt (argc, argv, "vca:h")) != -1)
    {
        switch (opt) {
                case 'v':
                        func = CS_COUNT_VOWEL;
                        type = CS_INT_64;
                        break;
                case 'c':
                        func = CS_COUNT_CONSONANT;
                        type = CS_INT_64;
                        break;
                case 'a':
                        if (!strcmp(optarg , "int") || !strcmp(optarg, "integer")){
                            func = CS_AVG_INT;
                            type = CS_INT_32;
                        }
                        if (!strcmp(optarg, "double")){
                            func = CS_AVG_DOUBLE;
                            type = CS_DOUBLE_64;
                        }
                        break;
                case 'h':
                        print_help(argv[0]);
                        exit(EXIT_SUCCESS);
                        break;
                case '?':
                        print_help(argv[0]);
                        exit(EXIT_FAILURE);
                default:
                        func = CS_NOP;
                        type = CS_INT_64;
                        break;
        }

        if ((argc - optind) != 1)
        {
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* set by the application, should be set by CSFS daemon */
    cs_set_status(CS_ON);
    int status = cs_get_status();
    if((status != CS_ON) && (status != CS_OFF)){
        fprintf(stderr, "Computational Storage Status Error: %d \n", status);
        exit(EXIT_FAILURE);
    }

    if (status == CS_ON){
        printf ("Computational storage status: %d set to ON\n", status);
    }
    if (status == CS_OFF){
        printf ("Computational storage status: %d set to OFF\n", status);
    }
    strcpy(filename, argv[optind]);
    printf ("Opening file %s \n", filename);
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fstat(fd, &file_stat);
    printf ("%s is %ld byte long\n", filename, file_stat.st_size);
    buf = (char*)malloc(file_stat.st_size);
    if (buf == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    nb_byte = read(fd, buf, file_stat.st_size);
    if (nb_byte != file_stat.st_size) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf ("Proceeding to IOCTL %s on file %s\n", CS_FNCT_NAME[func], filename);
    cs_compad.in_bfsz = nb_byte;
    cs_compad.fct_id = func;
    cs_compad.type_t = type;
    cs_compad.offset = 0;
    // the structure cs_compad is not preserved across ioctl call
    ret = ioctl(fd, CS_OPT, &cs_compad);
    type = cs_compad.type_t;
    if (ret != 0) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    switch (type) {
        case CS_CHAR:
            printf ("Result of: %s IOCTL on %s set to %c \n", CS_FNCT_DESC[func], filename, (int) cs_compad.out_bf.c);
            break;
        case CS_INT_32:
            printf ("Result of: %s IOCTL on %s set to %d \n", CS_FNCT_DESC[func], filename, (int) cs_compad.out_bf.i32);
            break;
        case CS_INT_64:
            printf ("Result of: %s IOCTL on %s set to %ld \n", CS_FNCT_DESC[func], filename, (long int) cs_compad.out_bf.i64);
            break;
        case CS_UINT_32:
            printf ("Result of: %s IOCTL on %s set to %d \n", CS_FNCT_DESC[func], filename, (uint32_t) cs_compad.out_bf.ui32);
            break;
        case CS_UINT_64:
            printf ("Result of: %s IOCTL on %s set to %ld \n", CS_FNCT_DESC[func], filename, (uint64_t) cs_compad.out_bf.ui64);
            break;
        case CS_FLOAT_32:
            printf ("Result of: %s IOCTL on %s set to %f \n", CS_FNCT_DESC[func], filename, (float) cs_compad.out_bf.f32);
            break;
        case CS_DOUBLE_64:
            printf ("Result of: %s IOCTL on %s set to %f \n", CS_FNCT_DESC[func], filename, (double) cs_compad.out_bf.d64);
            break;
        default:
            printf ("Result of: %s IOCTL on %s set to %d \n", CS_FNCT_DESC[func], filename, (int) cs_compad.out_bf.i32);
            break;
    }

    close(fd);
    exit(EXIT_SUCCESS);
}
