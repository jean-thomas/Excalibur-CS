/*
 * Definition of the function portfolio supported by Excalibur Computational Storage
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include "EXCBR_CSFS_fnct.h"

int cs_status;

const char *CS_FNCT_NAME[CS_FNCT_END] = {
    [CS_UNDEF]                = "CS_NOP",
    [CS_COUNT_VOWEL]          = "CS_COUNT_VOWEL",
    [CS_COUNT_CONSONANT]      = "CS_COUNT_CONSONANT",
    [CS_GET_STATUS]           = "CS_GET_STATUS",
    [CS_SET_STATUS]           = "CS_SET_STATUS",
    [CS_MAX]                  = "CS_MAX",
    [CS_MIN]                  = "CS_MIN",
    [CS_SUM]                  = "CS_SUM",
    [CS_PROD]                 = "CS_PROD",
    [CS_LAND]                 = "CS_LAND",
    [CS_LOR]                  = "CS_LOR",
    [CS_BAND]                 = "CS_BAND",
    [CS_BOR]                  = "CS_BOR",
    [CS_MAXLOC]               = "CS_MAXLOC",
    [CS_MINLOC]               = "CS_MINLOC",
    [CS_AVG_INT]              = "CS_AVG_INT",
    [CS_AVG_DOUBLE]           = "CS_AVG_DOUBLE",
    [CS_FIND_FIRST_OCCURENCE] = "CS_FIND_FIRST_OCCURENCE",
    [CS_NOP]                  = "CS_NOP",
};

const char *CS_FNCT_DESC[CS_FNCT_END] = {
    [CS_UNDEF]                = "this is not supported",
    [CS_COUNT_VOWEL]          = "returns the number of char elt being vowel",
    [CS_COUNT_CONSONANT]      = "Returns the number of char elt being consonant",
    [CS_GET_STATUS]           = "Set the status of Computational Storage either to CS_ON or CS_OFF",
    [CS_SET_STATUS]           = "Retrieve the current status of Compuational Storage, either CS_ON or CS_OFF",
    [CS_MAX]                  = "Returns the maximum element.",
    [CS_MIN]                  = "Returns the minimum element.",
    [CS_SUM]                  = "Sums the elements.",
    [CS_PROD]                 = "Multiplies all elements.",
    [CS_LAND]                 = "Performs a logical and across the elements.",
    [CS_LOR]                  = "Performs a logical or across the elements.",
    [CS_BAND]                 = "Performs a bitwise and across the bits of the elements.",
    [CS_BOR]                  = "Performs a bitwise or across the bits of the elements.",
    [CS_MAXLOC]               = "Returns the maximum value and the rank of the process that owns it.",
    [CS_MINLOC]               = "Who knowns?",
    [CS_AVG_INT]              = "Return the average value of length of byte seen as integer",
    [CS_AVG_DOUBLE]           = "Return the average value of length of byte seen as double",
    [CS_FIND_FIRST_OCCURENCE] = "Search function returning the position of the first occurence of a pattern",
    [CS_NOP]                  = "Return the number of byte read",
};

cs_fptr cs_cmd[CS_FNCT_END] = {
    [CS_UNDEF]                = cs_nop,
    [CS_COUNT_VOWEL]          = count_vowel,
    [CS_COUNT_CONSONANT]      = count_consonant,
    [CS_GET_STATUS]           = cs_nop,
    [CS_SET_STATUS]           = cs_nop,
    [CS_MAX]                  = d_cs_max,
    [CS_MIN]                  = d_cs_min,
    [CS_SUM]                  = cs_nop,
    [CS_PROD]                 = cs_nop,
    [CS_LAND]                 = cs_nop,
    [CS_LOR]                  = cs_nop,
    [CS_BAND]                 = cs_nop,
    [CS_BOR]                  = cs_nop,
    [CS_MAXLOC]               = cs_nop,
    [CS_MINLOC]               = cs_nop,
    [CS_AVG_INT]              = i_cs_average,
    [CS_AVG_DOUBLE]           = d_cs_average,
    [CS_FIND_FIRST_OCCURENCE] = cs_nop,
    [CS_NOP]                  = cs_nop,
};

static inline bool
is_vowel(char c)
{
    if (c =='a' || c =='e' || c =='i' || c =='o' || c =='u' || c =='y' ||
        c =='A' || c =='E' || c =='I' || c =='O' || c =='Y' || c =='Y')
        return true;

    return false;
}

void
count_vowel(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    char *my_buf = va_arg(valist, char *);
    size_t count = va_arg(valist, size_t);
    size_t *vowp = va_arg(valist, size_t *);

    size_t vow = 0;
    for (size_t i=0; i < count; i++)
    {
        if (is_vowel(my_buf[i]))
            vow++;
    }

    *vowp = vow;

    va_end(valist);
}

void
count_consonant(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    char *my_buf  = va_arg(valist, char *);
    size_t count  = va_arg(valist, size_t);
    size_t *consp = va_arg(valist, size_t *);

    size_t cons = 0;
    for (uint64_t i = 0; i < count; i++)
    {
        if (!is_vowel(my_buf[i]) && ((my_buf[i] >= 'a' && my_buf[i] <= 'z') ||
            (my_buf[i] >= 'A' && my_buf[i] <= 'Z')))
            cons++;
    }

    *consp = cons;

    va_end(valist);
}

void
cs_nop(int n_args, ...)
{
    (void)n_args;
}

void
i_cs_average(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    int *my_buf   = va_arg(valist, int *);
    size_t length = va_arg(valist, size_t);
    int *avgp     = va_arg(valist, int *);

    int avg      = 0;
    size_t count = length / sizeof(int);

    if (count == 0)
    {
        *avgp = 0;
        return;
    }

    fprintf(stderr, "Computing integer average value for an array of %ld elements\n", count);
    for (size_t i = 0; i < count; i++)
        avg += my_buf[i];

    fprintf(stderr, "i_cs_average: sum %i\n", avg);
    avg /= count;
    fprintf(stderr, "i_cs_average: results %d\n", avg);

    *avgp = avg;

    va_end(valist);
}

void
d_cs_average(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    double *my_buf = va_arg(valist, double *);
    size_t length  = va_arg(valist, size_t);
    double *avgp   = va_arg(valist, double *);

    double avg   = 0;
    size_t count = length / sizeof(double);

    if (count == 0)
    {
        *avgp = 0;
        va_end(valist);
        return;
    }

    fprintf(stderr, "Computing double average value for an array of %ld elements\n", count);
    for (size_t i = 0; i < count; i++)
    {
        avg += my_buf[i];
    }

    fprintf(stderr, "d_cs_average: sum %f\n", avg);
    avg = ((double) avg) / count;
    fprintf(stderr, "d_cs_average: results %f\n", avg);

    *avgp = avg;

    va_end(valist);
}

void
d_cs_min(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    double *my_buf = va_arg(valist, double *);
    size_t length   = va_arg(valist, size_t);
    double *minp   = va_arg(valist, double *);

    size_t count = length / sizeof(double);

    if (count == 0)
    {
        *minp = 0;
        va_end(valist);
        return;
    }

    double min = my_buf[0];
    for (size_t i = 0; i < count; i++)
    {
        fprintf(stderr, "Element %ld set to %f\n", i, my_buf[i]);
        if (min > my_buf[i])
            min = my_buf[i];
    }

    fprintf(stderr, "d_cs_min: min %f\n", min);

    *minp = min;

    va_end(valist);
}

void
d_cs_max(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    double *my_buf = va_arg(valist, double *);
    size_t length  = va_arg(valist, size_t);
    double *maxp   = va_arg(valist, double *);

    size_t count = length / sizeof(double);

    if (count == 0)
    {
        *maxp = 0;
        va_end(valist);
        return;
    }

    double max = my_buf[0];
    for (size_t i = 0; i < count; i++)
    {
        fprintf(stderr, "Element %ld set to %f\n", i, my_buf[i]);
        if (max < my_buf[i])
           max = my_buf[i];
    }

    fprintf(stderr, "d_cs_max: max %f\n", max);

    *maxp = max;

    va_end(valist);
}

void
find_first_occurence(int n_args, ...)
{
    assert(n_args == 3);

    va_list valist;
    va_start(valist, n_args);

    char *my_buf  = va_arg(valist, char *);
    char *pattern = va_arg(valist, char *);
    size_t *outp  = va_arg(valist, size_t *);

    char *ptr = strstr(my_buf, pattern);

    if (ptr != NULL)
        *outp = (ptr - my_buf);
    else
        *outp = 0;

    va_end(valist);
}
