/*
 * Excalibur Computational Storage:
 * List of supported function on the storage system
 *
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <stdarg.h>
#include "EXCBR_common.h"

#ifndef __CS_FUNC_HELPER__
#define __CS_FUNC_HELPER__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * List of implemented function
 */

extern void count_vowel (const struct cs_args_t *, struct cs_args_t *, void *);
extern void count_consonant(const struct cs_args_t *, struct cs_args_t *, void *);
extern void i_cs_average(const struct cs_args_t *, struct cs_args_t *, void *);
extern void d_cs_average(const struct cs_args_t *, struct cs_args_t *, void *);
extern void d_cs_min(const struct cs_args_t *, struct cs_args_t *, void *);
extern void d_cs_max(const struct cs_args_t *, struct cs_args_t *, void *);
extern void cs_nop(const struct cs_args_t *, struct cs_args_t *, void *); // this function just return the number of byte read

// Generic function pointer used to store all the supported function ptr
typedef void (*cs_fptr)(const struct cs_args_t *, struct cs_args_t *, void *);

extern cs_fptr cs_cmd[CS_FNCT_END];

#ifdef __cplusplus
}
#endif

#endif  // __CS_FUNC_HELPER__
