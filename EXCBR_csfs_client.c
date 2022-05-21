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

#include "EXCBR_CSFS_fnct.h"

const char *usage =
"Usage: mini_cs_client file\n"
"\n"
"Get size if <size> is omitted, set size otherwise\n"
"\n";

int main(int argc, char **argv)
{
	int fd;
	int nb_byte = -1;
	int ret = 0;
	char filename[PATH_MAX];
	struct stat file_stat;
	char buf[PATH_MAX];
	struct cs_fct_args_t cs_ioctl_args;

	if (argc < 2) {
		fprintf(stderr, "%s", usage);
		return 1;
	}

	cs_help_fnct();
	strcpy(filename, argv[1]);
	printf ("Opening file %s \n", filename);
	fd = open(filename, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	fstat(fd, &file_stat);
	printf ("%s is %ld byte long\n", filename, file_stat.st_size);
	if (file_stat.st_size > PATH_MAX) {
		file_stat.st_size = PATH_MAX;
		printf ("File is too large, read buffer size capped to %d \n", PATH_MAX);
	}

	nb_byte = read(fd, buf, file_stat.st_size);
	if (nb_byte != file_stat.st_size) {
		perror("read");
		return 1;
	}
	cs_ioctl_args.in_bfsz=nb_byte;
	cs_ioctl_args.fct_id= CS_AVG;
	cs_ioctl_args.type_t= CS_INT_32;
	printf ("Proceeding to IOCTL on file %s no function specified will be processed as a read \n", filename);
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("Return value of UNDEF IOCTL set to %d \n", ret);
	printf ("Result for average of %s set to %d \n", filename, (int) cs_ioctl_args.out_bf[0]);

	cs_ioctl_args.in_bfsz=nb_byte;
	cs_ioctl_args.fct_id= CS_AVG;
	cs_ioctl_args.type_t= CS_DOUBLE_64;
	printf ("Proceeding to IOCTL on file %s no function specified will be processed as a read \n", filename);
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("Result for average of %s set to %f \n", filename, (double) cs_ioctl_args.out_bf[0]);

	close(fd);
	return ret;

	cs_ioctl_args.in_bfsz=nb_byte;
	cs_ioctl_args.fct_id= CS_UNDEF;
	printf ("Proceeding to IOCTL on file %s no function specified will be processed as a read \n", filename);
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("Return value of UNDEF IOCTL set to %d \n", ret);

	printf ("Proceeding to IOCTL on file %s using **count vowel** function \n", filename);
	cs_ioctl_args.in_bfsz=nb_byte;
	cs_ioctl_args.type_t=CS_CHAR;
	cs_ioctl_args.fct_id= CS_COUNT_VOWEL;
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("IOCTL COUNT VOWEL return code: %d resumt: %ld\n", ret, cs_ioctl_args.out_bf[0]);

	printf ("Proceeding to IOCTL on file %s using **count consonant** function \n", filename);
	cs_ioctl_args.in_bfsz=nb_byte;
	cs_ioctl_args.fct_id= CS_COUNT_CONSONANT;
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("IOCTL COUNT CONSONANT return code: %d  result: %ld\n", ret, cs_ioctl_args.out_bf[0]);

	/*
	printf ("Proceeding to IOCTL on file %s using **Find first occurrence ** function \n", filename);
	cs_ioctl_args.fct_id= CS_FIND_FIRST_OCCURENCE;
	cs_ioctl_args.ext_arg= 1;
	char * my_pattern = strdup("text");
	cs_ioctl_args.ext_arg= 1;
	cs_ioctl_args.arglist= my_pattern;
	ret = ((cs_ptrc_sz_ptrc_to_int *) cs_cmd[cs_ioctl_args.fct_id])(buf, nb_byte, my_pattern);
	printf ("Find first occurence direct call returns: %d \n", ret);
	ret = ioctl(fd, CS_READ, &cs_ioctl_args);
	printf ("\nEnd of IOCTL on file %s \n", filename);
	*/
	close(fd);
	return ret;
}
