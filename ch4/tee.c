/*
 * The tee command reads its standar input until end-of-file, writing a
 * copy of the input to standard output and to the file named in its
 * command-line argument.  (We show an example of the use of this command
 * hwne we discuss FIFOs in Section 44.7.)  Implement tee using I/O
 * system calls.  By default, tee overwites any existing file with the
 * given name.  Impelment the -a command-line option (tee -a file), which
 * casuses te to append text to the end of a file  if it already exists.
 * (Refer to Appendix B for desription ofthe getopt() function, which can
 * be used to parse command-line options.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static int append_mode = 0;

int parse_args(int argc, char *argv[])
{
	char c;
	while ((c = getopt(argc, argv, "a")) != -1) {
		switch (c) {
		case 'a':
			append_mode = 1;
			break;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "Unkonw option `-%c'.\n", optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			abort();
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char buf[100];
	size_t len;
	char *file_mode;
    int i;
	FILE *files[20];
	int num_files;
	
	if (parse_args(argc, argv)) {
		return 1;
	}
	
	file_mode = (append_mode ? "a" : "w");
	
	num_files = argc - optind;
	if (num_files > 0) {
		if (files == NULL) {
			fprintf(stderr, "Unable to allocate file buffer space\n");
			return 1;
		}

		/* go through file arguments and either open for writing
		   or append based on the -a flag */
		for (i = optind; i < argc; i++) {
			FILE *pFile = fopen(argv[i], file_mode);
			if (pFile == NULL)
			{
				fprintf(stderr, "Unable to open file %s for mode %s",
					argv[i], file_mode);
				goto main_cleanup;
			}
			files[i - optind] = pFile; /* mind the offset */
		}
	}
	
	FILE *not_stdin = fopen("tee.c", "r");
	while ((len = fread(&buf[0], 1, sizeof(buf), not_stdin)) > 0) {
		fwrite(&buf[0], 1, len, stdout);
		for (i = 0; i < num_files; i++) {
			fwrite(&buf[0], 1, len, files[i]);
		}
	}

	main_cleanup:
	if (num_files > 0) {
		for (i = 0; i < num_files; i++) {
			fclose(files[i]);
		}
	}

	return 0;
}
