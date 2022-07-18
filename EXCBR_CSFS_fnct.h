/*
 * Excalibur Computational Storage:
 * List of supported function on the storage system
 *
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include "EXCBR_common.h"

#ifndef __CS_FUNC_HELPER__
#define __CS_FUNC_HELPER__
#define _GNU_SOURCE

/*
 * List of implemented function
 */
extern size_t count_vowel (void* buf, size_t count);
extern size_t count_consonant(void* buf, size_t count);
extern int i_cs_average(int* buf, size_t count);
extern double d_cs_average(double* buf, size_t count);
extern int find_first_occurence(void * buf, size_t count, char * pattern);
extern int cs_nop(void* buf, size_t count); // this function just return the number of byte read


// Generic function pointer used to store all the supported function ptr
typedef void* (*cs_fptr)(void);
/*
 * These typedef are introduced to cast back function
 * declated as generic function pointer the function array
 */
typedef char 	 cs_cptr_ui64_to_c  	(char*, size_t);
typedef void 	 cs_int_to_void      	(int);
typedef int 	 cs_void_to_i32 	    (void);
typedef int 	 cs_cptr_ui64_to_i32 	(char*, size_t);
typedef long int cs_cptr_ui64_to_i64 	(char*, size_t);
typedef uint32_t cs_cptr_ui64_to_ui32 	(char*, size_t);
typedef uint64_t cs_cptr_ui64_to_ui64 	(char*, size_t);
typedef float 	 cs_cptr_ui64_to_f32 	(char*, size_t);
typedef double 	 cs_cptr_ui64_to_d64 	(char*, size_t);

// cs_nop as place holder for functions yet to be implemented

cs_fptr cs_cmd[CS_FNCT_END] = { 
        [CS_COUNT_VOWEL]= (cs_fptr) count_vowel,
        [CS_COUNT_CONSONANT] 	= (cs_fptr) count_consonant,	
 	[CS_MAX] 	= (cs_fptr) cs_nop, 	
	[CS_MIN] 	= (cs_fptr) cs_nop, 
	[CS_SUM] 	= (cs_fptr) cs_nop, 
	[CS_PROD] 	= (cs_fptr) cs_nop,
	[CS_LAND] 	= (cs_fptr) cs_nop, 
	[CS_LOR]  	= (cs_fptr) cs_nop,
	[CS_BAND] 	= (cs_fptr) cs_nop, 
	[CS_BOR]	= (cs_fptr) cs_nop,
	[CS_MAXLOC] 	= (cs_fptr) cs_nop, 	
	[CS_MINLOC] 	= (cs_fptr) cs_nop,
 	[CS_AVG_INT]	= (cs_fptr) i_cs_average,
 	[CS_AVG_DOUBLE]	= (cs_fptr) d_cs_average,
 	[CS_NOP]	= (cs_fptr) cs_nop,
        [CS_FIND_FIRST_OCCURENCE] = (cs_fptr) cs_nop,
}; 

#endif  // __CS_FUNC_HELPER__
