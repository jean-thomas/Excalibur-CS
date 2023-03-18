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

static int cs_status;
void cs_set_status(int val)
{
    assert(val == CS_ON || val == CS_OFF);
    cs_status = val;
    return;
}

int cs_get_status(void)
{
    return cs_status;
}

static const char *CS_FNCT_NAME[CS_FNCT_END] = {
    [CS_UNDEF]           = "CS_NOP",
    [CS_COUNT_VOWEL]     = "CS_COUNT_VOWEL",
    [CS_COUNT_CONSONANT] = "CS_COUNT_CONSONANT",
    [CS_GET_STATUS]      = "CS_GET_STATUS",
    [CS_SET_STATUS]      = "CS_SET_STATUS",
    [CS_MAX]             = "CS_MAX",
    [CS_MIN]             = "CS_MIN",
    [CS_SUM]             = "CS_SUM",
    [CS_PROD]            = "CS_PROD",
    [CS_LAND]            = "CS_LAND",
    [CS_LOR]             = "CS_LOR",
    [CS_BAND]            = "CS_BAND",
    [CS_BOR]             = "CS_BOR",
    [CS_MAXLOC]          = "CS_MAXLOC",
    [CS_MINLOC]          = "CS_MINLOC",
    [CS_AVG_INT]         = "CS_AVG_INT",
    [CS_AVG_DOUBLE]      = "CS_AVG_DOUBLE",
    [CS_NOP]             = "CS_NOP",
};

static const char *CS_FNCT_DESC[CS_FNCT_END] = {
    [CS_UNDEF]           = "this is not supported",
    [CS_COUNT_VOWEL]     = "returns the number of char elt being vowel",
    [CS_COUNT_CONSONANT] = "Returns the number of char elt being consonant",
    [CS_GET_STATUS]      = "Set the status of Computational Storage either to CS_ON or CS_OFF",
    [CS_SET_STATUS]      = "Retrieve the current status of Compuational Storage, either CS_ON or CS_OFF",
    [CS_MAX]             = "Returns the maximum element.",
    [CS_MIN]             = "Returns the minimum element.",
    [CS_SUM]             = "Sums the elements.",
    [CS_PROD]            = "Multiplies all elements.",
    [CS_LAND]            = "Performs a logical and across the elements.",
    [CS_LOR]             = "Performs a logical or across the elements.",
    [CS_BAND]            = "Performs a bitwise and across the bits of the elements.",
    [CS_BOR]             = "Performs a bitwise or across the bits of the elements.",
    [CS_MAXLOC]          = "Returns the maximum value and the rank of the process that owns it.",
    [CS_MINLOC]          = "Who knowns?",
    [CS_AVG_INT]         = "Return the average value of length of byte seen as integer",
    [CS_AVG_DOUBLE]      = "Return the average value of length of byte seen as double",
    [CS_NOP]             = "Return the number of byte read",
};

const char *
cs_get_fnct_name(size_t id)
{
    assert(CS_NOP + 1);
    return CS_FNCT_NAME[id];
}

const char *
cs_get_fnct_desc(size_t id)
{
    assert(id < CS_NOP + 1);
    return CS_FNCT_DESC[id];
}

static inline bool
is_vowel(char c)
{
    if (c =='a' || c =='e' || c =='i' || c =='o' || c =='u' || c =='y' ||
        c =='A' || c =='E' || c =='I' || c =='O' || c =='Y' || c =='Y')
        return true;

    return false;
}

void
count_vowel(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);

    char *my_buf = data;
    size_t count = in_args->in_bfsz;

    size_t vow = 0;
    for (size_t i=0; i < count; i++)
    {
        if (is_vowel(my_buf[i]))
            vow++;
    }

    out->out_bf.ui32 = vow;
}

void
count_consonant(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);

    char *my_buf  = data;
    size_t count  = in_args->in_bfsz;

    size_t cons = 0;
    for (uint64_t i = 0; i < count; i++)
    {
        if (!is_vowel(my_buf[i]) && ((my_buf[i] >= 'a' && my_buf[i] <= 'z') ||
            (my_buf[i] >= 'A' && my_buf[i] <= 'Z')))
            cons++;
    }

    out->out_bf.ui32 = cons;
}

void
cs_nop(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    (void)in_args;
    (void)out;
    (void)data;
}

void
i_cs_average(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);

    int *my_buf   = data;
    size_t length = in_args->in_bfsz;

    int avg      = 0;
    size_t count = length / sizeof(int);

    if (count == 0)
    {
        out->out_bf.i32 = 0;
        return;
    }

    fprintf(stderr, "Computing integer average value for an array of %ld elements\n", count);
    for (size_t i = 0; i < count; i++)
        avg += my_buf[i];

    fprintf(stderr, "i_cs_average: sum %i\n", avg);
    avg /= count;
    fprintf(stderr, "i_cs_average: results %d\n", avg);

    out->out_bf.i32 = avg;
}

void
d_cs_average(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);

    double *my_buf = data;
    size_t length  = in_args->in_bfsz;

    double avg   = 0;
    size_t count = length / sizeof(double);

    if (count == 0)
    {
        out->out_bf.d64 = 0;
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

    out->out_bf.d64 = avg;
}

void
d_cs_min(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);

    double *my_buf = data;
    size_t length  = in_args->in_bfsz;
    size_t count = length / sizeof(double);

    if (count == 0)
    {
        out->out_bf.d64 = 0;
        return;
    }

    double min = my_buf[0];
    for (size_t i = 0; i < count; i++)
    {
        fprintf(stderr, "Element %ld set to %f\n", i, my_buf[i]);
        if (min > my_buf[i])
            min = my_buf[i];
    }
    out->out_bf.d64 = min;
}

void
d_cs_max(const cs_args_t *in_args, cs_args_t *out, void *data)
{
    assert(in_args);
    assert(out);
    assert(data);
    assert(out);

    double *my_buf = data;
    size_t length  = in_args->in_bfsz;

    size_t count = length / sizeof(double);

    if (count == 0)
    {
        out->out_bf.d64 = 0;
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

    out->out_bf.d64 = max;
}
