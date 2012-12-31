/*
 * Implement stripped down version of chmod that always do the equivalent
 * of chmod a+rX.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fs.h>

void print_usage()
{
	fprintf(stderr, "Usage: ./prog_chomod <filename/directory>\n");
	exit(EXIT_FAILURE);
}

/*
 * The a+rX modifier enables read permissions for all categories of
 * user and enables execute permissions for all cateogories of users.
 */
int chmod_directory(char *filename_or_directory, struct stat *st)
{
	mode_t st_mode;
	st_mode = st->st_mode;
	st_mode |= (S_IRUSR | S_IRGRP | S_IROTH |
		    S_IXUSR | S_IXGRP | S_IXOTH);
	chmod(filename_or_directory, st_mode);
	return 0;
}

/*
 * On files, the a+rX modifier enables read permissions for all categories
 * of user and enables execute permissions for all categories of user if
 * any categories of user already have execute privileges.
 */
int chmod_file(char *filename_or_directory, struct stat *st)
{
	mode_t st_mode;
	mode_t ixmask;
	st_mode = st->st_mode;

	/* enable read for all categories */
	st_mode |= (S_IRUSR | S_IXUSR | S_IRGRP);

	/* enable execute if enabled for any categories */
	ixmask = (S_IXUSR | S_IXGRP | S_IXOTH);
	if (st_mode & ixmask) {
		st_mode |= ixmask;
	}
	chmod(filename_or_directory, st_mode);
	return 0;
}


int main(int argc, char **argv)
{
	char *file_or_directory;
	struct stat st;
	int i;
	if (argc < 2) {
		print_usage();
	}

	for (i = 1; i < argc; i++) {
		file_or_directory = argv[i];
		if (stat(file_or_directory, &st) == -1) {
			fprintf(stderr, "File/Directory %s does not exist\n", file_or_directory);
			print_usage();
		}
		if (S_ISDIR(st.st_mode)) {
			chmod_directory(file_or_directory, &st);
		} else {
			chmod_file(file_or_directory, &st);
		}
	}
	return 0;
}
