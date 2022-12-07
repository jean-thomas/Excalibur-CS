
/*
 * Definition of the function portfolio supported by Excalibur Computational Storage
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

bool is_vowel(char c)
{
	if (c =='a' || c =='e' || c =='i' || c =='o' || c =='u' || c =='y' || c =='A' || c =='E' || c =='I' || c =='O' || c =='Y' || c =='Y')
		return true;
	return false;
}
size_t count_vowel(void * buf, size_t count)
{
        uint64_t i;
        size_t vow=0;
	char * my_buf = (char *) buf;

        for (i=0; i < count; i++)
        {
                if (is_vowel(my_buf[i]))
                        vow++;
        }
        return vow;
}

size_t count_consonant(void * buf, size_t count)
{
        uint64_t i;
        size_t cons=0;
	char * my_buf = (char *) buf;
        for (i=0; i < count; i++)
        {
                if (!is_vowel(my_buf[i]) && ((my_buf[i] >= 'a' && my_buf[i] <= 'z') || (my_buf[i] >= 'A' && my_buf[i] <= 'Z')))
                        cons++;
        }
        return cons;
}

size_t cs_nop(int * buf, size_t count)
{
        return (int)count;
}

int i_cs_average(char * buf, size_t length)
{
	size_t i;
	int avg = 0;
	int * my_buf = (int *) buf;
	size_t count = length / sizeof(int);

	if (count == 0)
		return 0;

	fprintf(stderr, "Computing integer average value for an array of %ld elements\n", count);
        for (i = 0; i < count; i++)
        {
                avg += my_buf[i];
        }
	fprintf(stderr, "i_cs_average: sum %i\n", avg);
	avg /= count;
	fprintf(stderr, "i_cs_average: results %d\n", avg);
        return avg;
}

double d_cs_average(char * buf, size_t length)
{
	size_t i;
	double avg = 0;
	double * my_buf = (double *) buf;
	size_t count = length / sizeof(double);

	if (count == 0)
		return 0;

	fprintf(stderr, "Computing double average value for an array of %ld elements\n", count);
        for (i = 0; i < count; i++)
        {
                avg += my_buf[i];
    }
fprintf(stderr, "d_cs_average: sum %f\n", avg);
avg = ((double) avg) / count;
fprintf(stderr, "d_cs_average: results %f\n", avg);
    return avg;
}

double d_cs_min(char * buf, size_t length)
{
	size_t i;
	double min = 0;
	double * my_buf = (double *) buf;
	size_t count = length / sizeof(double);

	if (count == 0)
		return 0;
    
    min = my_buf[0];
    for (i = 0; i < count; i++)
    {
	fprintf(stderr, "Element %ld set to %f\n", i, my_buf[i]);
        if (min > my_buf[i])
            min = my_buf[i];
    }

	fprintf(stderr, "d_cs_min: min %f\n", min);
    return min;
}

double d_cs_max(char * buf, size_t length)
{
	size_t i;
	double max = 0;
	double * my_buf = (double *) buf;
	size_t count = length / sizeof(double);

	if (count == 0)
		return 0;
    
    max = my_buf[0];
    for (i = 0; i < count; i++)
    {
	fprintf(stderr, "Element %ld set to %f\n", i, my_buf[i]);
        if (max < my_buf[i])
            max = my_buf[i];
    }

	fprintf(stderr, "d_cs_max: max %f\n", max);
    return max;
}
size_t find_first_occurence(void * buf, size_t count, char * pattern)
{
        char *ptr;
	char * my_buf = (char *) buf;
        ptr=strstr(my_buf, pattern);

        if (ptr != NULL)
                return (ptr - my_buf);
        else return 0;
}
