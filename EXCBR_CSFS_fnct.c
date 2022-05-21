
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
int count_vowel(void * buf, size_t count)
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

int count_consonant(void * buf, size_t count)
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


int cs_average(void * buf, size_t count)
{
	size_t i;
	int *my_buf = (int *) buf;
	int avg = 0;

	if (count == 0)
		return 0;

	fprintf(stderr, "Computing integer average value for an array of %ld elements\n", count);
        for (i = 0; i < count; i++)
        {
                avg += my_buf[i];
        }
	avg /= count;
        return avg;
}

int cs_nop(int * buf, size_t count)
{
        return (int)count;
}

int i_cs_average(int * buf, size_t count)
{
	size_t i;
	int avg = 0;

	if (count == 0)
		return 0;

	fprintf(stderr, "Computing integer average value for an array of %ld elements\n", count);
        for (i = 0; i < count; i++)
        {
                avg += buf[i];
        }
	avg /= count;
        return avg;
}

double d_cs_average(double * buf, size_t count)
{
	size_t i;
	double avg = 0;

	if (count == 0)
		return 0;

	fprintf(stderr, "Computing double average value for an array of %ld elements\n", count);
        for (i = 0; i < count; i++)
        {
                avg += buf[i];
        }
	avg /= count;
        return avg;
}

int find_first_occurence(void * buf, size_t count, char * pattern)
{
        char *ptr;
	char * my_buf = (char *) buf;
        ptr=strstr(my_buf, pattern);

        if (ptr != NULL)
                return (ptr - my_buf);
        else return 0;
}


