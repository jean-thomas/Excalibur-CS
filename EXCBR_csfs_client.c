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

#include "EXCBR_cs_client_app_helper.h"

const char *usage =
"Usage: ./EXCBR_csfs_client file\n"
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
	char *buf;
	struct cs_args_t cs_compad;

	if (argc < 2) {
		fprintf(stderr, "%s", usage);
		// cs_help_fnct();
		return 1;
	}

	strcpy(filename, argv[1]);
	printf ("Opening file %s \n", filename);
	fd = open(filename, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	fstat(fd, &file_stat);
	printf ("%s is %ld byte long\n", filename, file_stat.st_size);
	buf = (char*)malloc(file_stat.st_size);
	if (buf == NULL){
		perror("malloc");
		return 1;
	}

	nb_byte = read(fd, buf, file_stat.st_size);
	if (nb_byte != file_stat.st_size) {
		perror("read");
		return 1;
	}
	cs_compad.in_bfsz=nb_byte;
	cs_compad.fct_id= CS_AVG;
	cs_compad.type_t= CS_INT_32;
	printf ("Proceeding to IOCTL Average as integer value on file %s\n", filename);
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("Return value of UNDEF IOCTL set to %d \n", ret);
	printf ("Result for average of %s set to %d \n", filename, (int) cs_compad.out_bf[0]);

	cs_compad.in_bfsz=nb_byte;
	cs_compad.fct_id= CS_AVG;
	cs_compad.type_t= CS_DOUBLE_64;
	printf ("Proceeding to IOCTL Average as double value on file %s\n", filename);
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("Result for average of %s set to %f \n", filename, (double) cs_compad.out_bf[0]);

	close(fd);
	return ret;

	cs_compad.in_bfsz=nb_byte;
	cs_compad.fct_id= CS_UNDEF;
	printf ("Proceeding to IOCTL on file %s no function specified will be processed as a read \n", filename);
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("Return value of UNDEF IOCTL set to %d \n", ret);

	printf ("Proceeding to IOCTL on file %s using **count vowel** function \n", filename);
	cs_compad.in_bfsz=nb_byte;
	cs_compad.type_t=CS_CHAR;
	cs_compad.fct_id= CS_COUNT_VOWEL;
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("IOCTL COUNT VOWEL return code: %d resumt: %ld\n", ret, cs_compad.out_bf[0]);

	printf ("Proceeding to IOCTL on file %s using **count consonant** function \n", filename);
	cs_compad.in_bfsz=nb_byte;
	cs_compad.fct_id= CS_COUNT_CONSONANT;
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("IOCTL COUNT CONSONANT return code: %d  result: %ld\n", ret, cs_compad.out_bf[0]);

	/*
	printf ("Proceeding to IOCTL on file %s using **Find first occurrence ** function \n", filename);
	cs_compad.fct_id= CS_FIND_FIRST_OCCURENCE;
	cs_compad.ext_arg= 1;
	char * my_pattern = strdup("text");
	cs_compad.ext_arg= 1;
	cs_compad.arglist= my_pattern;
	ret = ioctl(fd, CS_OPT, &cs_compad);
	printf ("Find first occurence direct call returns: %d \n", ret);
	printf ("\nEnd of IOCTL on file %s \n", filename);
	*/
	close(fd);
	return ret;
}
