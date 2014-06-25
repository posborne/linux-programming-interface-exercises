/*
 * Write a program that opens and existing file for writing with the
 * O_APPEND flag, and then seeks to the beginning of the file before
 * writing some data.  Where does the data appear in the file?  Why?
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define PATTERN "Hello, world!"

int main(int argc, char *argv[])
{
	int fd;

	/* File opened as append */
	if (argc != 2) {
		printf("Must specify a file (argument 1)\n");
		return 1;
	}
	fd = open(argv[1], O_RDWR | O_APPEND);

	/* attempt to seek to the beginning of the file */
	lseek(fd, 0, SEEK_SET);

	/* Now, write some bytes */
	write(fd, PATTERN, sizeof(PATTERN));
	close(fd);
	return 0;
}
