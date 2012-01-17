/*
 * vector_fileio.c
 *
 * Implemented readv() and writev() using read() and write(), and
 * suitable functions from the malloc package (Section 7.1.2).
 *
 *
 *  Created on: Jan 16, 2012
 *      Author: posborne
 */

#include <assert.h>
#include <sys/uio.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

/*
 * A custom implementation of writev without nice things like support
 * for atomic operation  (use the real thing for that)
 *
 * DESCRIPTION
 *
 * The writev() function shall be equivalent to write(), except as described
 * below. The writev() function shall gather output data from the iovcnt
 * buffers specified by the members of the iov array: iov[0], iov[1], ...,
 * iov[iovcnt-1]. The iovcnt argument is valid if greater than 0 and less
 * than or equal to {IOV_MAX}, as defined in <limits.h>.
 *
 * Each iovec entry specifies the base address and length of an area in memory
 * from which data should be written. The writev() function shall always write
 * a complete area before proceeding to the next.
 *
 * If fildes refers to a regular file and all of the iov_len members in the
 * array pointed to by iov are 0, writev() shall return 0 and have no other
 * effect. For other file types, the behavior is unspecified.
 *
 * If the sum of the iov_len values is greater than {SSIZE_MAX}, the operation
 * shall fail and no data shall be transferred.
 *
 * RETURN VALUE
 *
 * Upon successful completion, writev() shall return the number of bytes
 * actually written. Otherwise, it shall return a value of -1, the file-pointer
 * shall remain unchanged, and errno shall be set to indicate an error.
 *
 * ERRORS
 *
 * Refer to write().
 *
 * In addition, the writev() function shall fail if:
 *
 * [EINVAL]
 * The sum of the iov_len values in the iov array would overflow an ssize_t.
 * The writev() function may fail and set errno to:
 *
 * [EINVAL]
 * The iovcnt argument was less than or equal to 0, or greater than {IOV_MAX}.
 *
 */
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
	/* NOTE: we assume that we aren't opened with O_APPEND or anything
	 * crazy like that.  That just won't work.
	 */
	int i, bytes_written;
	struct iovec *vec;
	unsigned long long cnt = 0;
	ssize_t total_bytes_written = 0;

	/* validate input values */
	if (iovcnt < 0) {
		errno = EINVAL;
		return -1;
	}
	for (i = 0; i < iovcnt; i++) {
		vec = (struct iovec *)&iov[i];
		cnt += vec->iov_len;
		if (cnt > SSIZE_MAX) {
			errno = EINVAL;
			return -1;
		}
	}

	/* before starting, verify that the sum of lengths is not
	 * greater than the SSIZE_MAX
	 */

	for (i = 0; i < iovcnt; i++) {
		vec = (struct iovec *)&iov[i];
		bytes_written = write(fildes, vec->iov_base, vec->iov_len);
		if (bytes_written < 0) {
			return bytes_written;
		} else {
			total_bytes_written += bytes_written;
		}
	}
	return total_bytes_written;
}

/*
 * Implementation of readv from uio.h without atomic guarantees of the
 * system call by the same name
 *
 * DESCRIPTION
 *
 * The readv() function shall be equivalent to read(), except as described
 * below. The readv() function shall place the input data into the iovcnt
 * buffers specified by the members of the iov array: iov[0], iov[1], ...,
 * iov[ iovcnt-1]. The iovcnt argument is valid if greater than 0 and less than
 * or equal to {IOV_MAX}.
 *
 * Each iovec entry specifies the base address and length of an area in memory
 * where data should be placed. The readv() function shall always fill an area
 * completely before proceeding to the next.
 *
 * Upon successful completion, readv() shall mark for update the st_atime field
 * of the file.
 *
 * RETURN VALUE
 *
 * Refer to read().
 *
 * ERRORS
 *
 * Refer to read().
 *
 * In addition, the readv() function shall fail if:
 *
 * [EINVAL]
 * The sum of the iov_len values in the iov array overflowed an ssize_t.
 *
 * [EINVAL]
 * The iovcnt argument was less than or equal to 0, or greater than {IOV_MAX}.
 *
 */
ssize_t readv(int fildes, const struct iovec *iov, int iovcnt)
{
	int i, bytes_read;
	struct iovec *vec;
	unsigned long long cnt = 0;
	ssize_t total_bytes_read = 0;

	if (iovcnt <= 0) {
		errno = EINVAL;
		return -1;
	}
	for (i = 0; i < iovcnt; i++) {
		vec = (struct iovec *)&iov[i];
		cnt += vec->iov_len;
		if (cnt > SSIZE_MAX) {
			errno = EINVAL;
			return -1;
		}
	}

	for (i = 0; i < iovcnt; i++) {
		vec = (struct iovec *)&iov[i];
		bytes_read = read(fildes, vec->iov_base, vec->iov_len);
		if (bytes_read < 0) {
			return bytes_read;
		} else {
			total_bytes_read += bytes_read;
		}
	}

	return total_bytes_read;
}


#define NUMBER_VECS (100)
#define CHUNK_SIZE (16)

static struct iovec read_iovecs[NUMBER_VECS];
static struct iovec write_iovecs[NUMBER_VECS];

int main(int argc, char *argv[])
{
	int i;

	/* Create buffers (malloc) */
	for	 (i = 0; i < NUMBER_VECS; i++) {
		int j;
		struct iovec *vec = (struct iovec *)&write_iovecs[i];
		void * mem;

		mem = malloc(CHUNK_SIZE);
		memset(mem, 0, CHUNK_SIZE);
		read_iovecs[i].iov_base = mem;
		read_iovecs[i].iov_len = CHUNK_SIZE;

		mem = malloc(CHUNK_SIZE);
		memset(mem, i, CHUNK_SIZE);
		write_iovecs[i].iov_base = mem;
		write_iovecs[i].iov_len = CHUNK_SIZE;
	}

	/* Write the data to the file */
	int fd = open(argv[1], (O_WRONLY | O_CREAT), (S_IRUSR | S_IWUSR));
	ssize_t bytes_written = writev(fd, &write_iovecs[0], NUMBER_VECS);
	assert (bytes_written == (NUMBER_VECS * CHUNK_SIZE));
	close(fd);

	/* Read it back now */
	fd = open(argv[1], (O_RDONLY));
	ssize_t bytes_read = readv(fd, &read_iovecs[0], NUMBER_VECS);
	assert (bytes_read == (NUMBER_VECS * CHUNK_SIZE));

	/* Compare and assert equal */
	for (i = 0; i < NUMBER_VECS; i++) {
		assert(memcmp((void *)write_iovecs[i].iov_base,
					   (void *)read_iovecs[i].iov_base,
					   CHUNK_SIZE) == 0);
	}
	printf("Done... no assertion errors\n");

	return 0;
}
