/*
 * This exercise is designed to demonstrate why the atomicity guaranteed
 * by opening a file with the O_APPEND flag is necessary.  Write a
 * program that takes up to three command-line arguments:
 *
 *     $ atomic_append <filename> <num-bytes> [x]
 *
 * This program should open the specified filename (creating it if
 * necessary) and append num-bytes bytes to the file by using write() to
 * write a byte at a time.  By default, the program should open the file
 * with the O_APPEND flag, but if a third command-line argument (x) is
 * supplied, th ethe O_APPEND flag should be omitted, and instead, the
 * program should perform and lseek(fd, 0, SEEK_END) call before each
 * write().  Run two instances of this program at the same time without
 * the x argument to write 1 million bytes to the same file:
 *
 *     $ atomic_append f1 1000000 & atomic_append f1 1000000
 *
 * Repeat the same steps, writing to a different file, but this time
 * specifying the x argument:
 *
 *     $ atomic_append f2 1000000 x & atomic_append f2 1000000 x
 *
 * List the sizes of the files f1 and f2 using `ls -l` and explain the
 * difference.
 *
 * Author: Paul Osborne <osbpau@gmail.com>
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int print_usage(char *argv[])
{
	printf("%s <filename> <num-bytes> [x]\n", argv[0]);
	printf("   where x is only checked for existence\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	int flags;
	int i;
	long num_bytes;
	char *filename;

	if (argc < 3 || argc > 4) {
		print_usage(argv);
		return 1;
	}

	flags = (O_RDWR | O_CREAT);
	if (argc == 3) {
		flags |= (O_APPEND);
	}

	filename = argv[1];
	num_bytes = atol(argv[2]);
	fd = open(filename, flags, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		printf("Could not open specified filename (%s)\n", filename);
		print_usage(argv);
		return 1;
	}

	for (i = 0; i < num_bytes; i++) {
		if (argc == 3) {
			write(fd, "i", 1);
		} else {
			lseek(fd, 0, SEEK_END);
			write(fd, "i", 1);
		}
	}

	close(fd);
	return 0;
}
