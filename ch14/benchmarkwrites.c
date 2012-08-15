/*
 * Write a program that measures the time required to create and then
 * remove a large number of 1-byte files from a single directory.  The
 * program should creaqte files with names of the form xNNNNNN where
 * NNNNNN is replaced by a random six-digit number.  The file should be
 * created in the random order in which their names are generated, and
 * then deleted in increasing numberical order (i.e., an order that is
 * different from that in which they were created).  The number of files
 * (NF) and the directory in which they are to be created should be
 * specifiable on the commaqnd line.  Measure the times required for
 * diffferent values of NF (e.g., in the range from 10000 to 20,0000) and
 * for different file systems (e.g. ext2, ext3, and XFS).  What patterns
 * do you observe on each file system as NF increases?  Do the results
 * change if the files are created in increasing numerical order
 * (x000001, x000002, x000003, and so on) and then be deleted inthe same
 * order?  If so, what do you think the reason(s) might be?  Again, do
 * the results var across file-system types?
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>

/* contstants */
#define FILE_CONTENTS "z"
#define DEFAULT_NUMBER_FILES 10000
#define FILENAME_FMT "%s/x%06d"
#define MAX_NUMBER_FILES 1000000
#define MODE_777 (S_IRUSR | S_IWUSR | S_IXUSR |\
                  S_IRGRP | S_IWGRP | S_IXGRP |\
                  S_IROTH | S_IWOTH | S_IXOTH)

/* globals */
static int number_files = DEFAULT_NUMBER_FILES;
static bool randomize = false;
static char dirname[1028];
static int * file_numbers; /* array of actual numbers */
static int * ordered_file_numbers; /* array of ordered numbers */

/* simple shuffle implementation lifted from: http://stackoverflow.com/a/6127606
 *
 * Arrange the N elements of ARRAY in random order.
 * Only effective if N is much smaller than RAND_MAX;
 * if this may not be the case, use a better random
 * number generator.
 */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

/* parse command-line options */
static int parse_options(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, "n:r")) != -1) {
		switch (c) {
		case 'n':
			number_files = atoi(optarg);
			break;
		case 'r':
			randomize = true;
			break;
		default:
			return 1;
		}
	}
	if (++optind != argc) {
		return 1;
	} else {
		strcpy(dirname, argv[argc - 1]);
	}
	return 0;
}

/* print usage to stderro for the application */
static void print_usage(int argc, char **argv)
{
	fprintf(stderr, "Usage: %s [options] <directory>\n", argv[0]);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -n <number_files> (Set the number of files to use in the test)\n");
	fprintf(stderr, "  -r                (Create random files and delete in a different order)\n");
}

/* run the benchmark test.  If all goes well return 0 */
static int runbenchmark()
{
	int i;
	int file_number;
	int fd;
	char filepath[1024];
	struct stat dirstat;
	struct timeval st_time, end_time;

	/* allocate memory for numbers */
	if ((file_numbers = (int *)malloc(sizeof(int) * number_files)) == NULL ||
	    (ordered_file_numbers = (int *)malloc(sizeof(int) * number_files)) == NULL) {
		fprintf(stderr, "Unable to allocate buffer for numbers\n");
		return 1;
	}

	/* check to see if the target directory exists, create if needed */
	if (stat(dirname, &dirstat)) {
		/* directory does not exist, attempt to create */
		if (mkdir(dirname, MODE_777)) {
			fprintf(stderr, "Unable to make directory %s, unable to continue\n", dirname);
			return 1;
		}
	} else if (!S_ISDIR(dirstat.st_mode)) {
		fprintf(stderr, "\"%s\" exists but is not a directory, unable to continue\n", dirname);
		return 1;
	}

	/* to eliminate error, calculate numbers first and put in array */
	for (i = 0; i < number_files; i++) {
		file_numbers[i] = i;
		ordered_file_numbers[i] = i;
	}
	if (randomize) {
		shuffle(file_numbers, number_files);
	}

	/* now, create the files, timing the operation */
	gettimeofday(&st_time, NULL);
	for (i = 0; i < number_files; i++) {
		sprintf(filepath, FILENAME_FMT, dirname, file_numbers[i]);
		fd = open(filepath, O_CREAT, MODE_777);
		if (fd == -1) {
			fprintf(stderr, "Unable to open file \"%s\" - ", filepath);
			perror("");
			return 1;
		}
		write(fd, FILE_CONTENTS, 1);
		close(fd);
	}
	
	/* now, delete in numerical order (just brute force it in
	 * order).  This is O(m*n) right now which is sort of slow but
	 * it should still be eclipsed by the time when we hit disk,
	 * which is what we are most concerned about.
	 */
	for (i = 0; i < number_files; i++) {
		sprintf(filepath, FILENAME_FMT, dirname, ordered_file_numbers[i]);
		if (unlink(filepath)) {
			fprintf(stderr, "Unable to unlink \"%s\" - ", filepath);
			perror("");
			return 1;
		}
	}
	gettimeofday(&end_time, NULL);

	printf("== Write/Delete Benchmark ==\n");
	printf("NF = %d\n", number_files);
	if (randomize) {
		printf("randomized = true\n");
	} else {
		printf("randomized = false\n");
	}
	printf("time: %0.3f secs\n",
	       (float)((int)(end_time.tv_sec - st_time.tv_sec)) +
	       (float)(int)(end_time.tv_usec - st_time.tv_usec) / 1000000);

	return 0;
}


int main(int argc, char **argv)
{
	if (parse_options(argc, argv)) {
		fprintf(stderr, "Error: Invalid options specified\n");
		print_usage(argc, argv);
		return 1;
	}
	return runbenchmark();
}
