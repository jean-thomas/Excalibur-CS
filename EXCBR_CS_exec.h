/*
 *  Excalibur Computational Storage ioctl internals
 */

#ifndef __CS_EXCBR_CS_exec
#define __CS_EXCBR_CS_exec

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "EXCBR_CSFS_fnct.h"

void
cs_exec(fuse_req_t req, struct fuse_file_info *fi, const void *in_buf);

#ifdef __cplusplus
}
#endif

#endif  // __CS_EXCBR_CS_exec
