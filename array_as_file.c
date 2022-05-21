
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

void print_help(char *prog)
	{
	fprintf (stderr,"Usage: %s [options] <file name> <number of values> \n", prog);
	fprintf (stderr,"Options:\n");
	fprintf (stderr,"-r read, read array and dispplay array from file\n");
	fprintf (stderr,"-w write, write array to file (default)\n");
	fprintf (stderr,"-c check, write array to file and check content\n");
	fprintf (stderr,"-f filename, set file name\n");
	fprintf (stderr,"-l length, number of array elements\n");
	fprintf (stderr,"-t type, data type: int or double 'default)\n");
	fprintf (stderr,"-h help, display this help message:\n");
	return; 
	}

int main(int argc, char **argv)
{
	size_t count;
	char *type = NULL;
	char *filename;
	int read = 0;
	int write = 0;
	int check = 0;
	int opt;

	while ((opt = getopt (argc, argv, "hrwct:")) != -1)
    	switch (opt)
      	{
      		case 'h':
        		print_help(argv[0]);
			exit(EXIT_SUCCESS);
        		break;
      		case 'r':
			read = 1;
        		break;
      		case 'w':
			write = 1;
        		break;
      		case 'c':
			check = 1;
			break;
      		case 't':
			if (!strcmp(optarg , "int") || !strcmp(optarg, "integer"))
				type = strdup("int");
			if (!strcmp(optarg, "double"))
				type = strdup("double");
			break;
      		case '?':
        		print_help(argv[0]);
			exit(EXIT_FAILURE);
      		default:
			write = 1;
			type = strdup("int");
			fprintf(stderr, "Default of switch\n");
			break;
      	}

	if ((argc - optind) != 2)
	{
		print_help(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (!type)
	{
		fprintf(stderr, "unrecognized data type: switch to double\n");
		type = strdup("double");
	}
	if (!read && ! write && !check)
	{
		fprintf(stderr, "unspecified operation: set to write mode\n");
		write = 1;
	}

	filename = strdup(argv[optind]);
	count = atoi(argv[optind+1]);

	if (write || check)
	{
		FILE *fptr = fopen(filename, "w");
		if (fptr == NULL)
		{
			fprintf (stderr,"Unable to open file %s in write mode\n", filename);
			exit (EXIT_FAILURE);
		}
		if (!strcmp(type, "int"))
		{
			for (size_t i = 0; i < count ; ++i)
			{
				// for debugging purpose, we do not use random data
				int ival = (int) i;
    				fwrite(&ival, sizeof(int), 1, fptr);
			}
		}
		else // default data type is double
		{
			for (size_t i = 0; i < count ; ++i)
			{
				// for debugging purpose, we do not use random data
				double dval = ((double) i) / 10;
    				fwrite(&dval, sizeof(double), 1, fptr);
			}
		}
		fclose(fptr);
	}

	if (read || check)
	{
		FILE *fptr = fopen(filename, "r");
		if (fptr == NULL)
		{
			fprintf (stderr,"Unable to open file %s in read mode\n", filename);
			exit (EXIT_FAILURE);
		}
		if (!strcmp(type, "int"))
		{
			for (size_t i = 0; i < count ; ++i)
			{
		 		int ival = 0;
    				fread(&ival, sizeof(int), 1, fptr);
				fprintf(stdout, "[%ld]: %d\n",i, ival);
			}
		}
		else // default is double
		{
		for (size_t i = 0; i < count ; ++i)
			{
				double dval = 0;
    				fread(&dval, sizeof(double), 1, fptr);
				fprintf(stdout, "[%ld]: %f\n",i, dval);
			}
		}
		fclose(fptr);
	}
	exit (EXIT_SUCCESS);
}
