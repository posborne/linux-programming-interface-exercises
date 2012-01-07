/*
 * Write a program like cp that, when used to copy a regular file that
 * contains holes (sequences of null bytes), also creates correspdonding
 * holes in the target file.
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

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
		printf("Failed to open input file: %s\n", strerror(inputFd));
		print_usage();
		return 1;
	}
	
	outputFd = open(argv[2], openFlags, filePerms);
	if (outputFd < 0) {
		printf("Failed to open output file: %s\n", strerror(inputFd));
		print_usage();
		return 1;
	}
	
	/* do the copy operation */
	while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
		if (write(outputFd, buf, numRead) != numRead) {
			printf("Failed to write full buffer to output file!\n");
			return 1;
		}
	}

	/* close fds */
	close(inputFd);
	close(outputFd);

	return 0;
}
