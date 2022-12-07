/*
 *  Excalibur Computational Storage Fuse client 
 *
 *  This file adapated from original work of Miklos Szeredi <miklos@szeredi.hu>
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
int cs_status;

/*
 * Command line options for Computational Storage File System
 *
 */
static struct cs_config {
        const char *cs;
} cs_config;

#define CS_OPTION(t, p)                           \
    { t, offsetof(struct cs_config, p), 1 }
static const struct fuse_opt cs_options[] = {
        CS_OPTION("--cs=%s", cs),
        FUSE_OPT_END
};

/* We are re-using pointers to our `struct lo_inode` and `struct
   lo_dirp` elements as inodes. This means that we must be able to
   store uintptr_t values in a fuse_ino_t variable. The following
   incantation checks this condition at compile time. */
#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 6) && !defined __cplusplus
_Static_assert(sizeof(fuse_ino_t) >= sizeof(uintptr_t),
	       "fuse_ino_t too small to hold uintptr_t values!");
#else
struct _uintptr_to_must_hold_fuse_ino_t_dummy_struct \
	{ unsigned _uintptr_to_must_hold_fuse_ino_t:
			((sizeof(fuse_ino_t) >= sizeof(uintptr_t)) ? 1 : -1); };
#endif

struct lo_inode {
	struct lo_inode *next; /* protected by lo->mutex */
	struct lo_inode *prev; /* protected by lo->mutex */
	int fd;
	ino_t ino;
	dev_t dev;
	uint64_t refcount; /* protected by lo->mutex */
};


struct lo_data {
	pthread_mutex_t mutex;
	int debug;
	const char *source;
	struct lo_inode root; /* protected by lo->mutex */
};

static const struct fuse_opt lo_opts[] = {
	{ "source=%s",
	  offsetof(struct lo_data, source), 0 },

	FUSE_OPT_END
};

static void fuse_cs_help(void)
{
	printf(
"    --cs=<on|off>	   (default \"off \")\n");
}
static void passthrough_ll_help(void)
{
	printf(
"    -o source=/home/dir    Source directory to be mounted\n");
}

static struct lo_data *lo_data(fuse_req_t req)
{
	return (struct lo_data *) fuse_req_userdata(req);
}

static struct lo_inode *lo_inode(fuse_req_t req, fuse_ino_t ino)
{
	if (ino == FUSE_ROOT_ID)
		return &lo_data(req)->root;
	else
		return (struct lo_inode *) (uintptr_t) ino;
}

static int lo_fd(fuse_req_t req, fuse_ino_t ino)
{
	return lo_inode(req, ino)->fd;
}

static bool lo_debug(fuse_req_t req)
{
	return lo_data(req)->debug != 0;
}

static void lo_init(void *userdata,
		    struct fuse_conn_info *conn)
{

	if(conn->capable & FUSE_CAP_EXPORT_SUPPORT)
		conn->want |= FUSE_CAP_EXPORT_SUPPORT;

}

static void lo_destroy(void *userdata)
{
	struct lo_data *lo = (struct lo_data*) userdata;

	while (lo->root.next != &lo->root) {
		struct lo_inode* next = lo->root.next;
		lo->root.next = next->next;
		free(next);
	}
}

static void lo_getattr(fuse_req_t req, fuse_ino_t ino,
			     struct fuse_file_info *fi)
{
	int res;
	struct stat buf;

	(void) fi;

	res = fstatat(lo_fd(req, ino), "", &buf, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return (void) fuse_reply_err(req, errno);

	fuse_reply_attr(req, &buf, 0);
}

static void lo_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
		       int valid, struct fuse_file_info *fi)
{
	int saverr;
	char procname[64];
	struct lo_inode *inode = lo_inode(req, ino);
	int ifd = inode->fd;
	int res;

	if (valid & FUSE_SET_ATTR_MODE) {
		if (fi) {
			res = fchmod(fi->fh, attr->st_mode);
		} else {
			sprintf(procname, "/proc/self/fd/%i", ifd);
			res = chmod(procname, attr->st_mode);
		}
		if (res == -1)
			goto out_err;
	}
	if (valid & (FUSE_SET_ATTR_UID | FUSE_SET_ATTR_GID)) {
		uid_t uid = (valid & FUSE_SET_ATTR_UID) ?
			attr->st_uid : (uid_t) -1;
		gid_t gid = (valid & FUSE_SET_ATTR_GID) ?
			attr->st_gid : (gid_t) -1;

		res = fchownat(ifd, "", uid, gid,
			       AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
		if (res == -1)
			goto out_err;
	}
	if (valid & FUSE_SET_ATTR_SIZE) {
		if (fi) {
			res = ftruncate(fi->fh, attr->st_size);
		} else {
			sprintf(procname, "/proc/self/fd/%i", ifd);
			res = truncate(procname, attr->st_size);
		}
		if (res == -1)
			goto out_err;
	}
	if (valid & (FUSE_SET_ATTR_ATIME | FUSE_SET_ATTR_MTIME)) {
		struct timespec tv[2];

		tv[0].tv_sec = 0;
		tv[1].tv_sec = 0;
		tv[0].tv_nsec = UTIME_OMIT;
		tv[1].tv_nsec = UTIME_OMIT;

		if (valid & FUSE_SET_ATTR_ATIME_NOW)
			tv[0].tv_nsec = UTIME_NOW;
		else if (valid & FUSE_SET_ATTR_ATIME)
			tv[0] = attr->st_atim;

		if (valid & FUSE_SET_ATTR_MTIME_NOW)
			tv[1].tv_nsec = UTIME_NOW;
		else if (valid & FUSE_SET_ATTR_MTIME)
			tv[1] = attr->st_mtim;

		if (fi)
			res = futimens(fi->fh, tv);
		else {
			sprintf(procname, "/proc/self/fd/%i", ifd);
			res = utimensat(AT_FDCWD, procname, tv, 0);
		}
		if (res == -1)
			goto out_err;
	}

	return lo_getattr(req, ino, fi);

out_err:
	saverr = errno;
	fuse_reply_err(req, saverr);
}

static struct lo_inode *lo_find(struct lo_data *lo, struct stat *st)
{
	struct lo_inode *p;
	struct lo_inode *ret = NULL;

	pthread_mutex_lock(&lo->mutex);
	for (p = lo->root.next; p != &lo->root; p = p->next) {
		if (p->ino == st->st_ino && p->dev == st->st_dev) {
			assert(p->refcount > 0);
			ret = p;
			ret->refcount++;
			break;
		}
	}
	pthread_mutex_unlock(&lo->mutex);
	return ret;
}

static int lo_do_lookup(fuse_req_t req, fuse_ino_t parent, const char *name,
			 struct fuse_entry_param *e)
{
	int newfd;
	int res;
	int saverr;
	struct lo_data *lo = lo_data(req);
	struct lo_inode *inode;

	memset(e, 0, sizeof(*e));

	newfd = openat(lo_fd(req, parent), name, O_PATH | O_NOFOLLOW);
	if (newfd == -1)
		goto out_err;

	res = fstatat(newfd, "", &e->attr, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		goto out_err;

	inode = lo_find(lo_data(req), &e->attr);
	if (inode) {
		close(newfd);
		newfd = -1;
	} else {
		struct lo_inode *prev, *next;

		saverr = ENOMEM;
		inode = calloc(1, sizeof(struct lo_inode));
		if (!inode)
			goto out_err;

		inode->refcount = 1;
		inode->fd = newfd;
		inode->ino = e->attr.st_ino;
		inode->dev = e->attr.st_dev;

		pthread_mutex_lock(&lo->mutex);
		prev = &lo->root;
		next = prev->next;
		next->prev = inode;
		inode->next = next;
		inode->prev = prev;
		prev->next = inode;
		pthread_mutex_unlock(&lo->mutex);
	}
	e->ino = (uintptr_t) inode;

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "  %lli/%s -> %lli\n",
			(unsigned long long) parent, name, (unsigned long long) e->ino);

	return 0;

out_err:
	saverr = errno;
	if (newfd != -1)
		close(newfd);
	return saverr;
}

static void lo_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	struct fuse_entry_param e;
	int err;

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "lo_lookup(parent=%" PRIu64 ", name=%s)\n",
			parent, name);

	err = lo_do_lookup(req, parent, name, &e);
	if (err)
		fuse_reply_err(req, err);
	else
		fuse_reply_entry(req, &e);
}

/*
 * Creates files on the underlying file system in response to a FUSE_MKNOD
 * operation
 */

static int mknod_wrapper(int dirfd, const char *path, const char *link,
        int mode, dev_t rdev)
{
        int res;

        if (S_ISREG(mode)) {
                res = openat(dirfd, path, O_CREAT | O_EXCL | O_WRONLY, mode);
                if (res >= 0)
                        res = close(res);
        } else if (S_ISDIR(mode)) {
                res = mkdirat(dirfd, path, mode);
        } else if (S_ISLNK(mode) && link != NULL) {
                res = symlinkat(link, dirfd, path);
        } else if (S_ISFIFO(mode)) {
                res = mkfifoat(dirfd, path, mode);
        } else {
                res = mknodat(dirfd, path, mode, rdev);
        }

        return res;
}



static void lo_mknod_symlink(fuse_req_t req, fuse_ino_t parent,
			     const char *name, mode_t mode, dev_t rdev,
			     const char *link)
{
	int res;
	int saverr;
	struct lo_inode *dir = lo_inode(req, parent);
	struct fuse_entry_param e;

	res = mknod_wrapper(dir->fd, name, link, mode, rdev);

	saverr = errno;
	if (res == -1)
		goto out;

	saverr = lo_do_lookup(req, parent, name, &e);
	if (saverr)
		goto out;

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "  %lli/%s -> %lli\n",
			(unsigned long long) parent, name, (unsigned long long) e.ino);

	fuse_reply_entry(req, &e);
	return;

out:
	fuse_reply_err(req, saverr);
}

static void lo_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name,
		     mode_t mode)
{
	lo_mknod_symlink(req, parent, name, S_IFDIR | mode, 0, NULL);
}

static void lo_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	int res;

	res = unlinkat(lo_fd(req, parent), name, AT_REMOVEDIR);

	fuse_reply_err(req, res == -1 ? errno : 0);
}

static void lo_rename(fuse_req_t req, fuse_ino_t parent, const char *name,
		      fuse_ino_t newparent, const char *newname,
		      unsigned int flags)
{
	int res;

	if (flags) {
		fuse_reply_err(req, EINVAL);
		return;
	}

	res = renameat(lo_fd(req, parent), name,
			lo_fd(req, newparent), newname);

	fuse_reply_err(req, res == -1 ? errno : 0);
}

static void lo_unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
{
	int res;

	res = unlinkat(lo_fd(req, parent), name, 0);

	fuse_reply_err(req, res == -1 ? errno : 0);
}

static void unref_inode(struct lo_data *lo, struct lo_inode *inode, uint64_t n)
{
        if (!inode)
                return;

        pthread_mutex_lock(&lo->mutex);
        assert(inode->refcount >= n);
        inode->refcount -= n;
        if (!inode->refcount) {
                struct lo_inode *prev, *next;

                prev = inode->prev;
                next = inode->next;
                next->prev = prev;
                prev->next = next;

                pthread_mutex_unlock(&lo->mutex);
                close(inode->fd);
                free(inode);

        } else {
                pthread_mutex_unlock(&lo->mutex);
        }
}


static void lo_forget_one(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
{
	struct lo_data *lo = lo_data(req);
	struct lo_inode *inode = lo_inode(req, ino);

	if (lo_debug(req)) {
		fuse_log(FUSE_LOG_DEBUG, "  forget %lli %lli -%lli\n",
			(unsigned long long) ino,
			(unsigned long long) inode->refcount,
			(unsigned long long) nlookup);
	}

	unref_inode(lo, inode, nlookup);
}

struct lo_dirp {
	DIR *dp;
	struct dirent *entry;
	off_t offset;
};

static struct lo_dirp *lo_dirp(struct fuse_file_info *fi)
{
	return (struct lo_dirp *) (uintptr_t) fi->fh;
}

static void lo_opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	int error = ENOMEM;
	struct lo_dirp *d;
	int fd;

	d = calloc(1, sizeof(struct lo_dirp));
	if (d == NULL)
		goto out_err;

	fd = openat(lo_fd(req, ino), ".", O_RDONLY);
	if (fd == -1)
		goto out_errno;

	d->dp = fdopendir(fd);
	if (d->dp == NULL)
		goto out_errno;

	d->offset = 0;
	d->entry = NULL;

	fi->fh = (uintptr_t) d;

	fuse_reply_open(req, fi);
	return;

out_errno:
	error = errno;
out_err:
	if (d) {
		if (fd != -1)
			close(fd);
		free(d);
	}
	fuse_reply_err(req, error);
}

static int is_dot_or_dotdot(const char *name)
{
	return name[0] == '.' && (name[1] == '\0' ||
				  (name[1] == '.' && name[2] == '\0'));
}

static void lo_do_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
			  off_t offset, struct fuse_file_info *fi, int plus)
{
	struct lo_dirp *d = lo_dirp(fi);
	char *buf;
	char *p;
	size_t rem = size;
	int err;

	(void) ino;

	buf = calloc(1, size);
	if (!buf) {
		err = ENOMEM;
		goto error;
	}
	p = buf;

	if (offset != d->offset) {
		seekdir(d->dp, offset);
		d->entry = NULL;
		d->offset = offset;
	}
	while (1) {
		size_t entsize;
		off_t nextoff;
		const char *name;

		if (!d->entry) {
			errno = 0;
			d->entry = readdir(d->dp);
			if (!d->entry) {
				if (errno) {  // Error
					err = errno;
					goto error;
				} else {  // End of stream
					break; 
				}
			}
		}
		nextoff = d->entry->d_off;
		name = d->entry->d_name;
		fuse_ino_t entry_ino = 0;
		if (plus) {
			struct fuse_entry_param e;
			if (is_dot_or_dotdot(name)) {
				e = (struct fuse_entry_param) {
					.attr.st_ino = d->entry->d_ino,
					.attr.st_mode = d->entry->d_type << 12,
				};
			} else {
				err = lo_do_lookup(req, ino, name, &e);
				if (err)
					goto error;
				entry_ino = e.ino;
			}

			entsize = fuse_add_direntry_plus(req, p, rem, name,
							 &e, nextoff);
		} else {
			struct stat st = {
				.st_ino = d->entry->d_ino,
				.st_mode = d->entry->d_type << 12,
			};
			entsize = fuse_add_direntry(req, p, rem, name,
						    &st, nextoff);
		}
		if (entsize > rem) {
			if (entry_ino != 0) 
				lo_forget_one(req, entry_ino, 1);
			break;
		}
		
		p += entsize;
		rem -= entsize;

		d->entry = NULL;
		d->offset = nextoff;
	}

    err = 0;
error:
    // If there's an error, we can only signal it if we haven't stored
    // any entries yet - otherwise we'd end up with wrong lookup
    // counts for the entries that are already in the buffer. So we
    // return what we've collected until that point.
    if (err && rem == size)
	    fuse_reply_err(req, err);
    else
	    fuse_reply_buf(req, buf, size - rem);
    free(buf);
}

static void lo_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
		       off_t offset, struct fuse_file_info *fi)
{
	lo_do_readdir(req, ino, size, offset, fi, 0);
}

static void lo_readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size,
			   off_t offset, struct fuse_file_info *fi)
{
	lo_do_readdir(req, ino, size, offset, fi, 1);
}

static void lo_releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	struct lo_dirp *d = lo_dirp(fi);
	(void) ino;
	closedir(d->dp);
	free(d);
	fuse_reply_err(req, 0);
}

static void lo_create(fuse_req_t req, fuse_ino_t parent, const char *name,
		      mode_t mode, struct fuse_file_info *fi)
{
	int fd;
	struct fuse_entry_param e;
	int err;

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "lo_create(parent=%" PRIu64 ", name=%s)\n",
			parent, name);

	fd = openat(lo_fd(req, parent), name,
		    (fi->flags | O_CREAT) & ~O_NOFOLLOW, mode);
	if (fd == -1)
		return (void) fuse_reply_err(req, errno);

	fi->fh = fd;
	
	fi->keep_cache = 1;

	err = lo_do_lookup(req, parent, name, &e);
	if (err)
		fuse_reply_err(req, err);
	else
		fuse_reply_create(req, &e, fi);
}

static void lo_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	int fd;
	char buf[64];

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "lo_open(ino=%" PRIu64 ", flags=%d)\n",
			ino, fi->flags);

	sprintf(buf, "/proc/self/fd/%i", lo_fd(req, ino));
	fd = open(buf, fi->flags & ~O_NOFOLLOW);
	if (fd == -1)
		return (void) fuse_reply_err(req, errno);

	fi->fh = fd;
	fuse_reply_open(req, fi);
}

static void lo_release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
	(void) ino;

	close(fi->fh);
	fuse_reply_err(req, 0);
}

static void lo_read(fuse_req_t req, fuse_ino_t ino, size_t size,
		    off_t offset, struct fuse_file_info *fi)
{
	struct fuse_bufvec buf = FUSE_BUFVEC_INIT(size);

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "lo_read(ino=%" PRIu64 ", size=%zd, "
			"off=%lu)\n", ino, size, (unsigned long) offset);

	buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
	buf.buf[0].fd = fi->fh;
	buf.buf[0].pos = offset;

	fuse_reply_data(req, &buf, FUSE_BUF_SPLICE_MOVE);
}

static void lo_write_buf(fuse_req_t req, fuse_ino_t ino,
			 struct fuse_bufvec *in_buf, off_t off,
			 struct fuse_file_info *fi)
{
	(void) ino;
	ssize_t res;
	struct fuse_bufvec out_buf = FUSE_BUFVEC_INIT(fuse_buf_size(in_buf));

	out_buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
	out_buf.buf[0].fd = fi->fh;
	out_buf.buf[0].pos = off;

	if (lo_debug(req))
		fuse_log(FUSE_LOG_DEBUG, "lo_write(ino=%" PRIu64 ", size=%zd, off=%lu)\n",
			ino, out_buf.buf[0].size, (unsigned long) off);

	res = fuse_buf_copy(&out_buf, in_buf, 0);
	if(res < 0)
		fuse_reply_err(req, -res);
	else
		fuse_reply_write(req, (size_t) res);
}

static void lo_statfs(fuse_req_t req, fuse_ino_t ino)
{
	int res;
	struct statvfs stbuf;

	res = fstatvfs(lo_fd(req, ino), &stbuf);
	if (res == -1)
		fuse_reply_err(req, errno);
	else
		fuse_reply_statfs(req, &stbuf);
}

static void lo_lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence,
		     struct fuse_file_info *fi)
{
	off_t res;

	(void)ino;
	res = lseek(fi->fh, off, whence);
	if (res != -1)
		fuse_reply_lseek(req, res);
	else
		fuse_reply_err(req, errno);
}

static void cs_ioctl(fuse_req_t req, fuse_ino_t ino, unsigned int cmd, void *arg, struct fuse_file_info *fi, unsigned int flags, const void *in_buf, size_t in_bufsz, size_t out_bufsz)
{

        /* read in arg */
	struct cs_args_t const *my_cs =  (struct cs_args_t *)in_buf;
	struct cs_args_t out_buf;
	char *read_bf;

	switch(cmd){	
		case CS_OPT:
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: fct_id  : %d\n", my_cs->fct_id);
			break;
	        default:
			fuse_log(FUSE_LOG_DEBUG, "\n non cs ioctl: exiting \n");
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
            		return;
	}
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: fct_id  : %d\n", my_cs->fct_id);
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: type_t  : %d\n", my_cs->type_t);
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: in_bfsz : %ld\n", my_cs->in_bfsz);
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: offset : %ld\n", my_cs->offset);
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: out_bfsz: %ld\n", my_cs->out_bfsz);

	read_bf = malloc(1 + my_cs->in_bfsz);
	size_t length = pread(fi->fh, read_bf, my_cs->in_bfsz, my_cs->offset);
	if (length == -1) {
		free (read_bf);
		fuse_reply_ioctl(req, ENOSYS, NULL , 0);
		return;
	}
	read_bf[length] = '\0'; // added just in case we want to display result on screen

	if ((my_cs->fct_id <= CS_UNDEF) || (my_cs->fct_id >= CS_FNCT_END)) {
		fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: unsuported command: %d\n", my_cs->fct_id);
		out_buf.type_t = CS_DOUBLE_64;
	   	out_buf.out_bf.ui64 = ((cs_cptr_ui64_to_ui64*) cs_cmd[CS_NOP])(read_bf, my_cs->in_bfsz);
		free (read_bf);
		fuse_reply_ioctl(req, 0, &out_buf, sizeof(struct cs_args_t));
		return;
	}
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: executing command: %d: %s\n", my_cs->fct_id, CS_FNCT_NAME[my_cs->fct_id]);
	// Calling the asked function and apply the cast in respect of type results and functiona arguments type
	switch (my_cs->type_t) {
/*
		case CS_VOID:
   	   		((cs_int_to_voidc *) cs_cmd[my_cs->fct_id])(read_bf);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: function with void return value\n", out_buf.out_bf.c);
			break;
*/
		case CS_CHAR:
   	   		out_buf.out_bf.c = ((cs_cptr_ui64_to_c *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: char : result set to %c\n", out_buf.out_bf.c);
			break;

		case CS_INT_32:
   	   		out_buf.out_bf.i32 = ((cs_cptr_ui64_to_i32 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: int 32 : result set to %d\n", out_buf.out_bf.i32);
			break;
		case CS_INT_64:
   	   		out_buf.out_bf.i64 = ((cs_cptr_ui64_to_i64 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: int 64 : result set to %d\n", out_buf.out_bf.i64);
			break;

		case CS_UINT_32:
   	   		out_buf.out_bf.ui32 = ((cs_cptr_ui64_to_ui32 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: unsigned int 32 : result set to %i\n", out_buf.out_bf.ui32);
			break;

		case CS_UINT_64:
   	   		out_buf.out_bf.i64 = ((cs_cptr_ui64_to_ui64 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: unsigned int 64 : result set to %ld\n", (size_t) out_buf.out_bf.ui64);
			break;

		case CS_FLOAT_32:
   	   		out_buf.out_bf.f32 =  ((cs_cptr_ui64_to_f32 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: float32 : result store as %f\n", out_buf.out_bf.f32);
			break;

		case CS_DOUBLE_64:
   	   		out_buf.out_bf.d64 =  ((cs_cptr_ui64_to_d64 *) cs_cmd[my_cs->fct_id])(read_bf, my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: double 64 : result store as %f\n", out_buf.out_bf.d64);
			break;
		default:
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: unsuported output type: %d\n", my_cs->type_t);
   	   		out_buf.out_bf.i32 = 0;
   	   		out_buf.type_t = my_cs->type_t;
			free (read_bf);
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
			return;
	}
	// send response to caller
  	out_buf.type_t = my_cs->type_t;
	free (read_bf);
	fuse_reply_ioctl(req, 0, &out_buf, sizeof(struct cs_args_t));
}

static void lo_flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
        int res;
        (void) ino;
        res = close(dup(fi->fh));
        fuse_reply_err(req, res == -1 ? errno : 0);
}


static const struct fuse_lowlevel_ops lo_oper = {
	.init		= lo_init,
	.destroy	= lo_destroy,
	.lookup		= lo_lookup,
	.mkdir		= lo_mkdir,
	.unlink		= lo_unlink,
	.rmdir		= lo_rmdir,
	.rename		= lo_rename,
	.getattr	= lo_getattr,
	.setattr	= lo_setattr,
	.opendir	= lo_opendir,
	.readdir	= lo_readdir,
	.readdirplus	= lo_readdirplus,
	.releasedir	= lo_releasedir,
	.create		= lo_create,
	.open		= lo_open,
	.release	= lo_release,
	.read		= lo_read,
	.write_buf      = lo_write_buf,
	.statfs		= lo_statfs,
	.lseek		= lo_lseek,
	.flush          = lo_flush,
	.ioctl          = cs_ioctl,
};

int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session *se;
	struct fuse_cmdline_opts opts;
	struct fuse_loop_config config;
	struct lo_data lo = { .debug = 0 };
	int ret = -1;

    cs_set_status(CS_ON);

	/* Don't mask creation mode, kernel already did that */
	umask(0);

	pthread_mutex_init(&lo.mutex, NULL);
	lo.root.next = lo.root.prev = &lo.root;
	lo.root.fd = -1;

	if (fuse_opt_parse(&args, &cs_config, cs_options, NULL) == -1)
		return 1;
	// is cs option is not set to 'on' then set it to off
	if ((cs_config.cs == NULL) || strcmp(cs_config.cs, "on") != 0)
			cs_config.cs = strdup("off");

	if (fuse_parse_cmdline(&args, &opts) != 0)
		return 1;
	if (opts.show_help) {
		printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
		fuse_cs_help();
		fuse_cmdline_help();
		fuse_lowlevel_help();
		passthrough_ll_help();
		ret = 0;
		goto err_out1;
	} else if (opts.show_version) {
		printf("FUSE library version %s\n", fuse_pkgversion());
		fuse_lowlevel_version();
		ret = 0;
		goto err_out1;
	}

	if(opts.mountpoint == NULL) {
		printf("usage: %s [options] <mountpoint>\n", argv[0]);
		printf("       %s --help\n", argv[0]);
		ret = 1;
		goto err_out1;
	}

	if (fuse_opt_parse(&args, &lo, lo_opts, NULL)== -1)
		return 1;

	lo.debug = opts.debug;
	lo.root.refcount = 2;
	if (lo.source) {
		struct stat stat;
		int res;

		res = lstat(lo.source, &stat);
		if (res == -1) {
			fuse_log(FUSE_LOG_ERR, "failed to stat source (\"%s\"): %m\n",
				 lo.source);
			exit(1);
		}
		if (!S_ISDIR(stat.st_mode)) {
			fuse_log(FUSE_LOG_ERR, "source is not a directory\n");
			exit(1);
		}

	} else {
		lo.source = "/tmp";
	}

	lo.root.fd = open(lo.source, O_PATH);
	if (lo.root.fd == -1) {
		fuse_log(FUSE_LOG_ERR, "open(\"%s\", O_PATH): %m\n",
			 lo.source);
		exit(1);
	}

	printf("Computational Storage set to %s\n",cs_config.cs);

	se = fuse_session_new(&args, &lo_oper, sizeof(lo_oper), &lo);
	if (se == NULL)
	    goto err_out1;

	if (fuse_set_signal_handlers(se) != 0)
	    goto err_out2;

	if (fuse_session_mount(se, opts.mountpoint) != 0)
	    goto err_out3;

	fuse_daemonize(opts.foreground);

	/* Block until ctrl+c or fusermount -u */
	if (opts.singlethread)
		ret = fuse_session_loop(se);
	else {
		config.clone_fd = opts.clone_fd;
		config.max_idle_threads = opts.max_idle_threads;
		ret = fuse_session_loop_mt(se, &config);
	}

	fuse_session_unmount(se);
err_out3:
	fuse_remove_signal_handlers(se);
err_out2:
	fuse_session_destroy(se);
err_out1:
	free(opts.mountpoint);
	fuse_opt_free_args(&args);

	if (lo.root.fd >= 0)
		close(lo.root.fd);

	return ret ? 1 : 0;
}
