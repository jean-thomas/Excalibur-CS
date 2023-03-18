/*
 *  Excalibur Computational Storage ioctl internals
 */

#define _GNU_SOURCE
#define FUSE_USE_VERSION 34

#include <fuse_lowlevel.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/xattr.h>

#include "EXCBR_CSFS_fnct.h"

cs_fptr cs_cmd[CS_FNCT_END] = {
    [CS_UNDEF]           = cs_nop,
    [CS_COUNT_VOWEL]     = count_vowel,
    [CS_COUNT_CONSONANT] = count_consonant,
    [CS_GET_STATUS]      = cs_nop,
    [CS_SET_STATUS]      = cs_nop,
    [CS_MAX]             = d_cs_max,
    [CS_MIN]             = d_cs_min,
    [CS_SUM]             = cs_nop,
    [CS_PROD]            = cs_nop,
    [CS_LAND]            = cs_nop,
    [CS_LOR]             = cs_nop,
    [CS_BAND]            = cs_nop,
    [CS_BOR]             = cs_nop,
    [CS_MAXLOC]          = cs_nop,
    [CS_MINLOC]          = cs_nop,
    [CS_AVG_INT]         = i_cs_average,
    [CS_AVG_DOUBLE]      = d_cs_average,
    [CS_NOP]             = cs_nop,
};

void
cs_exec(fuse_req_t req, struct fuse_file_info *fi, const void *in_buf)
{
	cs_args_t const *my_cs =  (cs_args_t *)in_buf;

	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: fct_id  : %d\n", my_cs->fct_id);
	switch(my_cs->fct_id)
	{
    	case CS_GET_STATUS:
		case CS_SET_STATUS:
   		case CS_SUM:
   		case CS_PROD:
   		case CS_LAND:
   		case CS_LOR:
   		case CS_BAND:
   		case CS_BOR:
   		case CS_MAXLOC:
   		case CS_MINLOC:
			fuse_log(FUSE_LOG_DEBUG, "\ncs ioctl does non yet implements func id %d: exiting \n",
				     my_cs->fct_id);
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
			return;

		case CS_COUNT_VOWEL:
		case CS_COUNT_CONSONANT:
		case CS_AVG_INT:
		case CS_AVG_DOUBLE:
		case CS_MAX:
    	case CS_MIN:

   			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: in_bfsz : %ld\n", my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: offset : %ld\n", my_cs->offset);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: out_bfsz: %ld\n", my_cs->out_bfsz);

			char *read_bf = malloc(1 + my_cs->in_bfsz);
			size_t length = pread(fi->fh, read_bf, my_cs->in_bfsz, my_cs->offset);

			if (length == -1) {
				free (read_bf);
				fuse_reply_ioctl(req, ENOSYS, NULL , 0);
				return;
			}
			read_bf[length] = '\0'; // added just in case we want to display result on screen

			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: executing command: %d: %s\n",
					 my_cs->fct_id, CS_FNCT_NAME[my_cs->fct_id]);

			cs_args_t out_buf = *my_cs;

			cs_cmd[my_cs->fct_id](my_cs, &out_buf, read_bf);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: command executed");

			free (read_bf);
			fuse_reply_ioctl(req, 0, &out_buf, sizeof(cs_args_t));
			break;

		default:
			fuse_log(FUSE_LOG_DEBUG, "\ncs ioctl does not support func id: %d: exiting \n",
					my_cs->fct_id);
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
	}
}
