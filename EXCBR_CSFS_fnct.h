/*
 * Excalibur Computational Storage:
 * List of supported function on the storage system
 *
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <inttypes.h>

#ifndef __CS_HELPER__
#define __CS_HELPER__
#define _GNU_SOURCE


typedef int CS_TYPE;
enum CS_TYPE {
	CS_CHAR,
        CS_INT_32,
        CS_INT_64,
        CS_FLOAT_32,
        CS_DOUBLE_64,
};

typedef int CS_FNCT_ID;

enum CS_FNCT_ID {
	CS_UNDEF = -1,
        CS_COUNT_VOWEL,		// returns the number of char elt being vowel
        CS_COUNT_CONSONANT,	// Returns the number of char elt being consonant
 	CS_MAX, 		// Returns the maximum element.
	CS_MIN, 		// Returns the minimum element.
	CS_SUM, 		// Sums the elements.
	CS_PROD, 		// Multiplies all elements.
	CS_LAND, 		// Performs a logical and across the elements.
	CS_LOR, 		// Performs a logical or across the elements.
	CS_BAND, 		// Performs a bitwise and across the bits of the elements.
	CS_BOR, 		// Performs a bitwise or across the bits of the elements.
	CS_MAXLOC, 		// Returns the maximum value and the rank of the process that owns it.
	CS_MINLOC, 		//
 	CS_AVG, 		// Return the average value
        CS_FIND_FIRST_OCCURENCE,// Search function returning the position of the first occurence of a pattern
        CS_FNCT_END,		// marker of the last supported function
};

//const char *CS_FNCT_NAME[] = {"CS_COUNT_VOWEL", "CS_COUNT_CONSONANT", "CS_MAX", "CS_MIN", "CS_SUM", "CS_PROD", "CS_LAND" , "CS_LOR", "CS_BAND", "CS_BOR", "CS_MAXLOC", "CS_MINLOC", "CS_AVG" , "CS_FIND_FIRST_OCCURENCE"};
const char *CS_FNCT_NAME[CS_FNCT_END] = {
	[CS_COUNT_VOWEL]= "CS_COUNT_VOWEL", 
        [CS_COUNT_CONSONANT] = "CS_COUNT_CONSONANT", 
 	[CS_MAX] 	= "CS_MAX",
	[CS_MIN] 	= "CS_MIN",
	[CS_SUM] 	= "CS_SUM",
	[CS_PROD] 	= "CS_PROD",
	[CS_LAND] 	= "CS_LAND",
	[CS_LOR]  	= "CS_LOR",
	[CS_BAND] 	= "CS_BAND",
	[CS_BOR]	= "CS_BOR",
	[CS_MAXLOC] 	= "CS_MAXLOC",
	[CS_MINLOC] 	= "CS_MINLOC",
 	[CS_AVG]	= "CS_AVG",
        [CS_FIND_FIRST_OCCURENCE] = "CS_FIND_FIRST_OCCURENCE",
	};

const char *CS_FNCT_DESC[CS_FNCT_END] ={
	[CS_COUNT_VOWEL] ="returns the number of char elt being vowel",
	[CS_COUNT_CONSONANT] = "Returns the number of char elt being consonant",
 	[CS_MAX]        = "Returns the maximum element.",
	[CS_MIN]	= "Returns the minimum element.",
	 [CS_SUM]       = "Sums the elements.",
	[CS_PROD]       = "Multiplies all elements.",
	[CS_LAND]       = "Performs a logical and across the elements.",
	[CS_LOR]        = "Performs a logical or across the elements.",
	[CS_BAND]       = "Performs a bitwise and across the bits of the elements.",
	[CS_BOR]        = "Performs a bitwise or across the bits of the elements.",
	[CS_MAXLOC]     = "Returns the maximum value and the rank of the process that owns it.",
	[CS_MINLOC]     = "Who knowns?",
	[CS_AVG]        = "Return the average value",
	[CS_FIND_FIRST_OCCURENCE] = "Search function returning the position of the first occurence of a pattern",
};

typedef int cs_ptrc_sz_to_int(void*, size_t);
extern int count_vowel (void* buf, size_t count);
extern int count_consonant(void* buf, size_t count);
extern int i_cs_average(int* buf, size_t count);
extern int f_cs_average(float* buf, size_t count);
extern double d_cs_average(double* buf, size_t count);
extern int cs_average(void* buf, size_t count);

// this function just return the number of byte read
extern int cs_nop(void* buf, size_t count);

typedef int cs_ptrc_sz_ptrc_to_int(void*, size_t, char*);
extern int find_first_occurence(void * buf, size_t count, char * pattern);

// generic function pointer used to store all the supported function ptr
typedef int (*cs_fp)(void);
// using c99 partial array initialization to register implemented function 

int (*cs_cmdlong[CS_FNCT_END])(void *, size_t) = { 
        [CS_COUNT_VOWEL]= count_vowel,
        [CS_COUNT_CONSONANT] 	= count_consonant,	
 	[CS_MAX] 	= cs_nop, 	
	[CS_MIN] 	= cs_nop, 
	[CS_SUM] 	= cs_nop, 
	[CS_PROD] 	= cs_nop,
	[CS_LAND] 	= cs_nop, 
	[CS_LOR]  	= cs_nop,
	[CS_BAND] 	= cs_nop, 
	[CS_BOR]	= cs_nop,
	[CS_MAXLOC] 	= cs_nop, 	
	[CS_MINLOC] 	= cs_nop,
 	[CS_AVG]	= cs_average,
        [CS_FIND_FIRST_OCCURENCE] = cs_nop,
}; 

int (*cs_cmd[2])(void *, size_t) = {count_vowel, count_consonant};


#define cs_average(buf, lenght) \
   _Generic((buff), int*: i_cs_average, float*: f_cs_average, double*: d_cs_average, default: i_cs_average)(buff, length)


struct cs_fct_args_t
	{
	CS_FNCT_ID fct_id;
	CS_TYPE type_t;
	size_t in_bfsz;
	size_t out_bfsz;
	uint64_t out_bf[64]; // current limit is 512 Byte
	} cs_fct_args;

/* Use 'E' as magic number */
#define CS_MAGIC 'E'
#define        CS_READ       _IOWR(CS_MAGIC, 1, cs_fct_args)


void cs_help_fnct()
{
        printf ("Functions supported by Compuational Storage\n");
        for (int i = CS_UNDEF + 1 ; i < CS_FNCT_END ; i++)
        {
                printf ("Name: %20s \t -- %s\n",CS_FNCT_NAME[i], CS_FNCT_DESC[i]);
        }
}




#endif  // __CS_HELPER__
