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

#include <sys/uio.h>


ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
	return 0;
}

ssize_t readv(int fildes, const struct iovec *iov, int iovcnt)
{
	return 0;
}

int main(int argc, char *argv[])
{
	return 0;
}
