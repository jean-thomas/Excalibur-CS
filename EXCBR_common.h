/*
  Excalibur Computation Storage:

  definition and declaration for the client application

*/

#ifndef __CS_COMMON___
#define __CS_COMMON___


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


typedef int CS_FNCT_ID;
enum CS_FNCT_ID {
        CS_UNDEF = -1,
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
        CS_FIND_FIRST_OCCURENCE,// Search function returning the position of the first occurence of a pattern
        CS_NOP,			// return the number of byte read
        CS_FNCT_END,            // marker of the last supported function
};

//const char *CS_FNCT_NAME[] = {"CS_COUNT_VOWEL", "CS_COUNT_CONSONANT", "CS_MAX", "CS_MIN", "CS_SUM", "CS_PROD", "CS_LAND" , "CS_LOR", "CS_BAND", "CS_BOR", "CS_MAXLOC", "CS_MINLOC", "CS_AVG" , "CS_FIND_FIRST_OCCURENCE"};
const char *CS_FNCT_NAME[CS_FNCT_END] = {
        [CS_COUNT_VOWEL]= "CS_COUNT_VOWEL",
        [CS_COUNT_CONSONANT] = "CS_COUNT_CONSONANT",
        [CS_GET_STATUS] = "CS_GET_STATUS",
        [CS_SET_STATUS] = "CS_SET_STATUS",
        [CS_MAX]        = "CS_MAX",
        [CS_MIN]        = "CS_MIN",
        [CS_SUM]        = "CS_SUM",
        [CS_PROD]       = "CS_PROD",
        [CS_LAND]       = "CS_LAND",
        [CS_LOR]        = "CS_LOR",
        [CS_BAND]       = "CS_BAND",
        [CS_BOR]        = "CS_BOR",
        [CS_MAXLOC]     = "CS_MAXLOC",
        [CS_MINLOC]     = "CS_MINLOC",
        [CS_AVG_INT]    = "CS_AVG_INT",
        [CS_AVG_DOUBLE] = "CS_AVG_DOUBLE",
        [CS_NOP]        = "CS_NOP",
        [CS_FIND_FIRST_OCCURENCE] = "CS_FIND_FIRST_OCCURENCE",
        };

const char *CS_FNCT_DESC[CS_FNCT_END] ={
        [CS_COUNT_VOWEL] ="returns the number of char elt being vowel",
        [CS_COUNT_CONSONANT] = "Returns the number of char elt being consonant",
        [CS_GET_STATUS] = "Set the status of Computational Storage either to CS_ON or CS_OFF",
        [CS_SET_STATUS] = "Retrieve the current status of Compuational Storage, either CS_ON or CS_OFF",
        [CS_MAX]        = "Returns the maximum element.",
        [CS_MIN]        = "Returns the minimum element.",
        [CS_SUM]        = "Sums the elements.",
        [CS_PROD]       = "Multiplies all elements.",
        [CS_LAND]       = "Performs a logical and across the elements.",
        [CS_LOR]        = "Performs a logical or across the elements.",
        [CS_BAND]       = "Performs a bitwise and across the bits of the elements.",
        [CS_BOR]        = "Performs a bitwise or across the bits of the elements.",
        [CS_MAXLOC]     = "Returns the maximum value and the rank of the process that owns it.",
        [CS_MINLOC]     = "Who knowns?",
        [CS_AVG_INT]    = "Return the average value of length of byte seen as integer",
        [CS_AVG_DOUBLE] = "Return the average value of length of byte seen as double",
        [CS_NOP]        = "Return the number of byte read",
        [CS_FIND_FIRST_OCCURENCE] = "Search function returning the position of the first occurence of a pattern",
};


/*
 * show list of supported function
 */
void cs_help_fnct()
{
        printf ("Functions supported by Compuational Storage\n");
        for (int i = CS_UNDEF + 1 ; i < CS_FNCT_END ; i++)
        {
                printf ("Name: %20s \t -- %s\n",CS_FNCT_NAME[i], CS_FNCT_DESC[i]);
        }
}

/*
 * These definitions are own independently by both client ans server.
 * Which means that clients do not communicate with server to check if 
 * if Computational Storage is active. This is a pure place holder / mock-up function
 */
#define CS_ON 1
#define CS_OFF 0
int cs_status;
void cs_set_status(int val)
{
    cs_status = val;
    return;
}

int cs_get_status(void)
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
struct cs_args_t
        {
        CS_FNCT_ID fct_id;
        CS_TYPE type_t;
        size_t in_bfsz;
        size_t out_bfsz;
        CS_RET out_bf; // current return limit is a single elt
        } cs_args;

#endif // __CS_COMMON__
