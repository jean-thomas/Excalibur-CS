#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "EXCBR_common.h"


double storage_delegate_op(int fd, size_t offset, size_t sz, char *op)
{
    struct cs_args_t cs_args;
    double ret;

    // we would like to get the fd from Python
    if (fcntl (fd, F_GETFD) == -1) 
        {
        perror("Corrupted file descriptor");
        exit(EXIT_FAILURE);
        }

    cs_args.in_bfsz = sz;
    cs_args.offset = offset;
    if (strncmp(op,"MAX",3))
        cs_args.fct_id=CS_MAX;
    else
        cs_args.fct_id = CS_MIN;
    cs_args.type_t = CS_DOUBLE_64;

    // the content of cs_args is not preserved across ioctl call
    ret = ioctl(fd, CS_OPT, &cs_args);
    cs_args.type_t = CS_DOUBLE_64;
    ret = (double) cs_args.out_bf.d64;

    return ret;
}
