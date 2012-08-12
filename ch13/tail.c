/* Implementation of tail command line progam with '-n' argument */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>

#define BUFFER_SIZE (1024 * 5)
#define DEFAULT_NUMBER_LINES 10

static int number_lines = DEFAULT_NUMBER_LINES;
static char *filename = NULL;
static char buf[BUFFER_SIZE];


void print_usage(int argc, char** argv)
{
	fprintf(stderr, "Usage: %s [-n lines] <file>\n", argv[0]);
}


int parse_options(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch (c) {
		case 'n':
			number_lines = atoi(optarg);
			break;
		case '?':
			if (optopt == 'n') {
				fprintf(stderr, "Option -%c requires an argument\n", optopt);
			} else if (isprint(optopt)) {
				fprintf(stderr, "Unknown option  `-%c'.\n", optopt);
			} else {
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			}
			return 1;
		default:
			abort(); /* this shouldn't happen */
		}
	}
	
	/* parse positional arg: filename */
	if ((optind + 1) != argc) {
		fprintf(stderr, "Positional argument filename not specified\n");
		print_usage(argc, argv);
		return 1;
	} else {
		filename = argv[optind];
	}
	return 0;
}

/* return the number of occurrences of c in buf */
int str_occurrences(char *buf, int c)
{
	int count = 0;
	char *pch;
	while ((pch = strchr(buf, c)) != NULL) {
		count++;
		buf = pch + 1;
	}
	return count;
}


/* return a pointer to the nth occurrence of c in buf */
char * str_occurrence_offset(const char *buf, int c, int n)
{
	int count = 0;
	char *pch;
	for (count = 0; count < n; count++) {
		pch = strchr(buf, c);
		if (pch == NULL) {
			return NULL;
		}
		buf = pch + 1;
	}
	return pch + 1;
}


int tail_file(int fd)
{
	int bytes_read;
	while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
		write(STDOUT_FILENO, buf, bytes_read);
	}
	return 0;
}


int main(int argc, char **argv)
{
	int fd, curline, offset, numbytes;
	char *nlptr;
	if (parse_options(argc, argv)) {
		print_usage(argc, argv);
		return 1;
	}
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Unable to open '%s'\n", filename);
		return 1;
	}

	/* now, scan from the end of the file back looking for a number of lines */
	offset = 0;
	numbytes = (int)(-sizeof(buf));
	curline = 0;
	do {
		numbytes = read(fd, buf, sizeof(buf));
		offset -= numbytes;
		curline += str_occurrences(buf, '\n');
		if (numbytes != sizeof(buf)) {
			break;
		}
	} while (curline <= number_lines && numbytes == sizeof(buf));

	if (curline >= number_lines) {
		nlptr = str_occurrence_offset(buf, '\n', curline - number_lines);
		offset += (int)((unsigned long)&nlptr[0] - (unsigned long)&buf[0]);
		lseek(fd, offset, SEEK_END);
		return tail_file(fd);
	} else {
		lseek(fd, 0, SEEK_SET);
	}
	return tail_file(fd);
}
