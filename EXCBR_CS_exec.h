/*
 *  Excalibur Computational Storage ioctl internals
 */

#ifndef __CS_EXCBR_CS_exec
#define __CS_EXCBR_CS_exec

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fuse3/fuse_lowlevel.h>
#include "EXCBR_CSFS_fnct.h"

#define FUSE_USE_VERSION 34

void cs_exec(fuse_req_t, const cs_args_t *, void *);

#ifdef __cplusplus
}
#endif

#endif  // __CS_EXCBR_CS_exec
