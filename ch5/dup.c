/*
 * dup.c
 *
 * Implement dup() and dup2() using fcntl() and, where necessary,
 * close().  (You may ignore the fact that dup2() and fcntl() return
 * different errno values for some error cases.)  For dup2(), remember to
 * handle the special case where oldfd equals newfd.  In this case, you
 * should check whether oldfd is valid, which can be done by, for
 * example, checking if fcntl(oldfd, F_GETFL) succeeds.  If oldfd is not
 * valid, then the function should return -1 with errrno set to EBADF.
 *
 * Here's a description of dup/dup2 from the man pages:
 *
 *     These system calls create a copy of the file descriptor oldfd.
 *
 *     dup() uses the lowest-numbered unused descriptor for the new descriptor.
 *
 *     dup2() makes newfd be the copy of oldfd, closing newfd first if
 *     necessary, but note the following:
 *
 *     *  If oldfd is not a valid file descriptor, then the call fails, and
 *        newfd is not closed.
 *
 *     *  If oldfd is a valid file descriptor, and newfd has the same value
 *        as oldfd, then dup2() does nothing, and returns newfd.
 *
 *     After a successful return from one of these system calls, the old and
 *     new file descriptors may be used interchangeably.  They refer to
 *     the same open file description (see open(2)) and thus share file offset
 *     and file status flags; for example, if the file offset is mod‐ified by
 *     using lseek(2) on one of the descriptors, the offset is also changed
 *     for the other.
 *
 *     The  two  descriptors  do not share file descriptor flags (the
 *     close-on-exec flag).  The close-on-exec flag (FD_CLOEXEC; see fcntl(2))
 *     for the duplicate descriptor is off.
 *
 *  Created on: Jan 15, 2012
 *      Author: Paul Osborne <osbpau@gmail.com>
 */
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/*
 * dup returns a new file descriptor that is a copy of the oldfd
 * or -1 if the operation cannot be completed.
 */
int dup(int oldfd)
{
	/* verify that oldfd is in fact a valid file descriptor */
	if (fcntl(oldfd, F_GETFD) < 0) {
		errno = EBADF;
		return -1;
	}
	return fcntl(oldfd, F_DUPFD, 0);
}

/*
 * dup2 creates a new file descriptor that is a copy of oldfd or
 * -1 if the operation cannot be completed.  oldfd will be copied
 * into newfd with the following being true:
 *   - if newfd is negative or greater than or equal to OPEN_MAX, the dup2()
 *     function returns a value -1
 *   - If oldfd is a valid file descriptor and is equal to newfd, the dup2()
 *     function returns newfd without closing it.
 *   - If oldfd is not a valid file descriptor, dup2() fails and does not
 *     close newfd.
 *   - If successful, dup2() returns a value that is equal to the value of
 *     newfd.  If a failure occurs, it returns a value of -1
 *
 * errno may be set the follow values on failure:
 *   - EBADF: fildes is not a valid open file descriptor
 *   - EINTR: A signal was caught during the dup2() system call
 *   - EMFILE: The number of file descriptors exceeds OPEN_MAX
 */
int dup2(int oldfd, int newfd)
{
	/* verify that oldfd is in fact a valid file descriptor */
	if (fcntl(oldfd, F_GETFD) < 0) {
		errno = EBADF;
		return -1;
	}

	/* Check to see if oldfd == newfd */
	if (oldfd == newfd) {
		return newfd; /* no need to do anything */
	}

	/* Check to see if newfd is open */
	int ret = fcntl(newfd, F_GETFD);
	if (ret < 0 && errno != EBADF) {
		return -1;
	} else if (ret >= 0) {
		close(newfd); /* TODO: check return code */
	}

	/* Hand things off to fcntl */
	return fcntl(oldfd, F_DUPFD, newfd);
	if (ret < 0) {
		return ret;
	} else if (ret != newfd) {
		close(ret); /* this is not the newfd we are looking for */
		return -1;
	} else {
		return ret;
	}
}

#define ENABLE_TEST (1)
#if ENABLE_TEST

int main(int argc, char *argv[])
{
	int fd1, fd2;
	char buf[30];
	memset(buf, '\0', sizeof(buf));
	fd1 = open(argv[1], O_RDONLY);
	printf("%s\n", buf);
	if (argc == 2) {
		fd2 = dup(fd1);
	}
	//assert(fcntl(fd1, F_GETFD) == fcntl(fd2, F_GETFD));
	read(fd1, &buf[0], 20);
	printf("%s", buf);
	read(fd2, &buf[0], 20);
	printf("%s\n", buf);
	close(fd1);
	close(fd2);
	return 0;
}
#endif // ENABLE_COMMAND_LINE
