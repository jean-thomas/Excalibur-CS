/*
  Excalibur Computation Storage:

  definition and declaration for the client application

*/

#ifndef __CS_COMMON___
#define __CS_COMMON___

#ifdef __cplusplus
extern "C" {
#endif

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

/* Use 'E' as magic number */
#define CS_MAGIC 'E'
#define        CS_OPT       _IOWR(CS_MAGIC, 1, cs_args)

enum CS_FNCT_ID {
    CS_UNDEF = 0,
    CS_COUNT_VOWEL,         // returns the number of char elt being vowel
    CS_COUNT_CONSONANT,     // Returns the number of char elt being consonant
    CS_GET_STATUS,          // Returns the current status of the CS feature, either enable or desable
    CS_SET_STATUS,          // Set the status of the CS feature, either to ON or OFF
    CS_MAX,                 // Returns the maximum element.
    CS_MIN,                 // Returns the minimum element.
    CS_SUM,                 // Sums the elements.
    CS_PROD,                // Multiplies all elements.
    CS_LAND,                // Performs a logical and across the elements.
    CS_LOR,                 // Performs a logical or across the elements.
    CS_BAND,                // Performs a bitwise and across the bits of the elements.
    CS_BOR,                 // Performs a bitwise or across the bits of the elements.
    CS_MAXLOC,              // Returns the maximum value and the rank of the process that owns it.
    CS_MINLOC,              //
    CS_AVG_INT,             // Return the average value for an array of integer
    CS_AVG_DOUBLE,          // Return the average value for an array of double
    CS_NOP,         // return the number of byte read
    CS_FNCT_END,            // marker of the last supported function
};

extern const char *CS_FNCT_NAME[CS_FNCT_END];

extern const char *CS_FNCT_DESC[CS_FNCT_END];

/*
 * show list of supported function
 */
static inline void cs_help_fnct()
{
    printf ("Functions supported by Compuational Storage\n");
    for (int i = CS_UNDEF + 1 ; i < CS_FNCT_END ; i++)
        printf ("Name: %20s \t -- %s\n",CS_FNCT_NAME[i], CS_FNCT_DESC[i]);
}

/*
 * These definitions are own independently by both client ans server.
 * Which means that clients do not communicate with server to check if
 * if Computational Storage is active. This is a pure place holder / mock-up function
 */
#define CS_ON  1
#define CS_OFF 0
extern int cs_status;
static inline void cs_set_status(int val)
{
    cs_status = val;
    return;
}

static inline int cs_get_status(void)
{
    return cs_status;
}

typedef enum cs_type {
        CS_CHAR,
        CS_INT_32,
        CS_INT_64,
        CS_UINT_32,
        CS_UINT_64,
        CS_FLOAT_32,
        CS_DOUBLE_64,
} CS_TYPE;

typedef union cs_ret {
        char 	 c;
        int 	 i32;
        long int i64;
        uint32_t ui32;
        uint64_t ui64;
        float    f32;
        double   d64;
        void* 	 ptr;
} CS_RET;

/*
 * In addition to this data structure,
 * CS will also receive a file handle
 * This structure is used to specified the
 * type of the operation to proceed on the
 * file descriptor
 */
extern struct cs_args_t {
    size_t  fct_id;
    size_t  in_bfsz;
    size_t  offset;
    size_t  out_bfsz;
    CS_RET  out_bf;
    int     status; // 0 for success, otherwise -1
} cs_args;

#ifdef __cplusplus
}
#endif

#endif // __CS_COMMON__
