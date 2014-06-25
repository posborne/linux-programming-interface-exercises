/*
 * This programs create a file with holes in it created
 * by seeking over bits of the file being written.  This
 * is a helper for cpholes.
 * 
 * Usage: ./prog_holyfile <num_gaps> <gap_size> <filename>
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PATTERN_CHUNK_SIZE 80
static const char *TEST_PATTERN = "1234567890_HelloWorld\n";

void print_usage()
{
  printf("Usage: ./prog_holyfile <num_gaps> <gap_size> <filename>\n");
}

int main(int argc, char *argv[])
{
	int outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	int numGaps, gapSize;
	int err;

	/* parse argv
	 * ./prog_cpholes src dst
	 */
	if (argc != 4 || strcmp(argv[1], "--help") == 0) {
		print_usage();
		return 1;
	}

	if ((numGaps = atoi(argv[1])) == 0 ||
	    (gapSize = atoi(argv[2])) == 0) {
		print_usage();
		return 1;
	}

	/* Open input and output files */
	openFlags = (O_CREAT | O_WRONLY | O_TRUNC);
	filePerms = (S_IRUSR | S_IWUSR | S_IRGRP |
		     S_IROTH | S_IWOTH);
	
	outputFd = open(argv[3], openFlags, filePerms);
	if (outputFd < 0) {
		printf("Failed to open output file: %s\n", strerror(errno));
		print_usage();
		return 1;
	}
	
	/* Fill the buffer with test pattern */
	int chunkIndex, i;
	for (chunkIndex = 0; chunkIndex < numGaps; chunkIndex++) {
		for (i = 0; i < PATTERN_CHUNK_SIZE; i++) {
			// printf("--> %d [%c]\n", (int)(i % strlen(TEST_PATTERN)), TEST_PATTERN[i % strlen(TEST_PATTERN)]);
			write(outputFd, &TEST_PATTERN[i % strlen(TEST_PATTERN)], 1);
		}
		if ((err = lseek(outputFd, gapSize, SEEK_CUR)) < 0) {
			printf("lseek err: %s\n", strerror(errno));
			return 1;
		}
	}

	/* close fds */
	close(outputFd);

	return 0;
}
