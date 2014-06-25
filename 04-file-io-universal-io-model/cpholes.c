/*
 * Write a program like cp that, when used to copy a regular file that
 * contains holes (sequences of null bytes), also creates corresponding
 * holes in the target file.
 *
 * In actuality, this application replaces any sequence of NUL bytes with
 * file holes.  This can be useful as a means of compression for files
 * with large NUL byte gaps.  When normal reads are performed on the files,
 * the result will be the same.
 *
 * Usage: ./prog_cpholes <src> <dst>
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#ifndef BUF_SIZE /* Allow "cc -D" to override */
#define BUF_SIZE 1024
#endif

void print_usage()
{
	printf("Usage: ./prog_cpholes <src> <dst>\n");
}

int main(int argc, char *argv[])
{
	int inputFd, outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];

	/* parse argv
	 * ./prog_cpholes src dst
	 */
	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		print_usage();
		return 1;
	}
	
	/* Open input and output files */
	openFlags = (O_CREAT | O_WRONLY | O_TRUNC);
	filePerms = (S_IRUSR | S_IWUSR | S_IRGRP |
		     S_IROTH | S_IWOTH);
	
	inputFd = open(argv[1], O_RDONLY);
	if (inputFd < 0) {
		printf("Failed to open input file: %s\n", strerror(errno));
		print_usage();
		return 1;
	}
	
	outputFd = open(argv[2], openFlags, filePerms);
	if (outputFd < 0) {
		printf("Failed to open output file: %s\n", strerror(errno));
		print_usage();
		return 1;
	}
	
	/*
	 * For copying, we read-and-copy from source to destination
	 * one byte at a time.  When we hit a NUL byte in the source.
	 * We start counting how many bytes are going to be in our
	 * file hole.
	 *
	 * NOTE: Linux 3.1 adds SEEK_DATA and SEEK_HOLE flags for
	 * lseek which would be quite nice here.  Unfortunately, my
	 * box (and most others) aren't running 3.1 yet.
	 *   $ uname -r
	 *   3.0.0-14-generic
	 */
	int i, err;
	unsigned long holeSize = 0;
	while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
		for (i = 0; i < numRead; i++) {
			if (buf[i] == '\0') {
				holeSize++;
			} else if (holeSize > 0) {
				lseek(outputFd, holeSize, SEEK_CUR);
				write(outputFd, &buf[i], 1);
				holeSize = 0;
			} else {
				write(outputFd, &buf[i], 1);
			}
		}
	}

	/* close fds */
	close(inputFd);
	close(outputFd);

	return 0;
}
