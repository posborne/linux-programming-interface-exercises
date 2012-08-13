/*
Write a program that lists all processes that have a particular file
pathname open.  This can be achieved by inspecting the contents of all
of the /proc/PID/fd/* symbolic links.  This will require nested loops
employing readdir(3) to scan all /proc/PID directories, and then the
contents of all /proc/PID/fd entries within each /proc/PID directory.
To read the contents of a /proc/PID/fd/n symbolic link requires the
use of readlink(), described in Section 18.5.

*/
#include <stdio.h>
#include <string.h>

static void print_usage(int argc, char **argv)
{
	fprintf(stderr, "Usage: %s <filepath>", argv[0]);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		print_usage(argc, argv);
		return 1;
	}
	
	return 0;
}
