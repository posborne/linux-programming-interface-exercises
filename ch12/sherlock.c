/*
 * Write a program that lists all processes that have a particular file
 * pathname open.  This can be achieved by inspecting the contents of all
 * of the /proc/PID/fd/* symbolic links.  This will require nested loops
 * employing readdir(3) to scan all /proc/PID directories, and then the
 * contents of all /proc/PID/fd entries within each /proc/PID directory.
 * To read the contents of a /proc/PID/fd/n symbolic link requires the
 * use of readlink(), described in Section 18.5.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#define PROCFS_ROOT "/proc"

static void print_usage(int argc, char **argv)
{
	fprintf(stderr, "Usage: %s <filepath>", argv[0]);
}

static int print_proc(char *dirname)
{
	char filename[256];
	char linebuf[256];
	char procname[256];
	char pid[32];
	char ppid[32];
	char *key;
	char *value;
	FILE *p_file;
	strcpy(filename, dirname);
	strcat(filename, "/status");
	p_file = fopen(filename, "r");
	if (p_file == NULL) {
		return 1; /* just ignore, this is fine I guess */
	}
	while (fgets(linebuf, sizeof(linebuf), p_file) != NULL) {
		key = strtok(linebuf, ":");
		value = strtok(NULL, ":");
		if (key != NULL && value != NULL) {
			trim(key); trim(value);
			if (strcmp(key, "Pid") == 0) {
				strcpy(pid, value);
			} else if (strcmp(key, "Name") == 0) {
				strcpy(procname, value);
			}
		}
	}
	printf("%s (pid: %s)\n", procname, pid);
	return 0;
}


static int check_process(char *dirname, char *match)
{
	char linkpath[256];
	char linktarget[1024];
	char linebuf[256];
	char procname[256];
	struct stat st;
	DIR *dirp;
	struct dirent *directory_entry;
	int len;

	/* first check that this dir has an fd subdir */
	strcpy(linkpath, dirname);
	strcat(linkpath, "/fd");
	if ((dirp = opendir(linkpath)) == NULL) {
		return 1;
	}
	do {
		if ((directory_entry = readdir(dirp)) != NULL) {
			if (directory_entry->d_type == DT_LNK) {
				strcpy(linkpath, dirname);
				strcat(linkpath, "/fd/");
				strcat(linkpath, directory_entry->d_name);
				len = readlink(linkpath, linktarget, sizeof(linktarget) - 1);
				if (len > 0) {
					linktarget[len] = '\0';
					if (strcmp(linktarget, match) == 0) {
						return 0;
					}
				}
			}
		}
	} while (directory_entry != NULL);
	return 1;
}


int main(int argc, char **argv)
{
	DIR *dirp;
	struct dirent *directory_entry;
	char dirname[256];

	if (argc != 2) {
		print_usage(argc, argv);
		return 1;
	}

	printf("Processes with handles to \"%s\":\n", argv[1]);

	/* walk through each /proc/PID entry */
	if ((dirp = opendir(PROCFS_ROOT)) == NULL) {
		perror("Unabled to open /proc");
		return 1;
	}
	do {
		if ((directory_entry = readdir(dirp)) != NULL) {
			if (directory_entry->d_type == DT_DIR) {
				strcpy(dirname, PROCFS_ROOT);
				strcat(dirname, "/");
				strcat(dirname, directory_entry->d_name);
				if (!check_process(dirname, argv[1])) {
					print_proc(dirname);
				}
			}
		}
	} while (directory_entry != NULL);

	return 0;
}
