/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                Copyright (C) Paul Osborne, 2012                      *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING for details.                      *
\**********************************************************************/

/* write_bytes.c

   Write bytes to a file. (A simple program for file I/O benchmarking.)

   Usage: write_bytes file num-bytes buf-size

   Writes 'num-bytes' bytes to 'file', using a buffer size of 'buf-size'
   for each write().

   If compiled with -DUSE_O_SYNC, open the file with the O_SYNC flag,
   so that all writes (data and metadata) are flushed to disk.

   If compiled with -DUSE_FDATASYNC, perform an fdatasync() after each write,
   so that data is flushed to the disk.

   If compiled with -DUSE_FSYNC, perform an fsync() after each write, so that
   data and metadata are flushed to the disk.
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>

/* #define USE_O_SYNC 1 */
/* #define O_SYNC 1 */
/* #define USE_FSYNC 1 */
/* #define USE_FDATASYNC 1 */

#define min(a, b) (a > b ? b : a)

int
main(int argc, char *argv[])
{
    size_t bufSize, numBytes, thisWrite, totWritten;
    char *buf;
    int fd, openFlags;

    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
      printf("%s file num-bytes buf-size\n", argv[0]);
      return 1;
    }

    numBytes = atoi(argv[2]);
    bufSize = atoi(argv[3]);

    buf = malloc(bufSize);
    if (buf == NULL) {
        printf("malloc\n");
	return 1;
    }

    openFlags = O_CREAT | O_WRONLY;

#if defined(USE_O_SYNC) && defined(O_SYNC)
    openFlags |= O_SYNC;
#endif

    fd = open(argv[1], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        printf("open\n");
	return 1;
    }

    for (totWritten = 0; totWritten < numBytes;
            totWritten += thisWrite) {
        thisWrite = min(bufSize, numBytes - totWritten);

        if (write(fd, buf, thisWrite) != thisWrite) {
            printf("partial/failed write\n");
	    return 1;
	}

#ifdef USE_FSYNC
        if (fsync(fd)) {
            printf("fsync");
	    return 1;
	}
#endif
#ifdef USE_FDATASYNC
        if (fdatasync(fd)) {
            printf("fdatasync");
	    return 1;
	}
#endif
    }

    if (close(fd) == -1) {
      printf("close");
      return 1;
    }

    return 0;
}
