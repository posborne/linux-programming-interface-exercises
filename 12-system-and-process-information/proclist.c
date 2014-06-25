/*
Assingment 12-1

Copyright 2012, Paul Osborne

Write a program that lists the porcess ID and command name for all
processes being run by the user named in the program's command-line
argument.  (You may find the userIdFromName() function from Listing
8-1, on page 159, useful.)  This can be done by inspecting the Name:
and Uid: lines of all of the /proc/PID/status files on the system.
Walking through all of the /proc/PID directories on the system
requires the use of readdir(3), which is described in Section 18.8.
Make sure your porgram correctly handles the possibility that a
/proc/PID directory disappears between the time that the porgram
determines that the directory exists and the time that it tries to
open the corresponding /proc/PID/status file.
*/
#include "proclib.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#define PROCFS_ROOT "/proc"


uid_t user_id_from_name(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;
	if (name == NULL || *name == '\0') {
		return -1;
	}

	u = strtol(name, &endptr, 10);
	if (*endptr == '\0') {
		return u;
	}

	pwd = getpwnam(name);
	if (pwd == NULL) {
		return -1;
	}

	return pwd->pw_uid;
}


void print_usage(int argc, char **argv)
{
	printf("Usage: %s <username>\n", argv[0]);
}


int check_dir(char *dirname, uid_t uid)
{
	char filename[256];
	char linebuf[256];
	char uidstr[128];
	char procname[256];
	char pid[32];
	int keepme;
	char *pstr;
	char *key;
	char *value;
	FILE *p_file;
	sprintf(uidstr, "%d", uid);
	strcpy(filename, dirname);
	strcat(filename, "/status");
	p_file = fopen(filename, "r");
	if (p_file == NULL) {
		return 1; /* just ignore, this is fine I guess */
	}
	keepme = 0;
	int res;
	while (fgets(linebuf, sizeof(linebuf), p_file) != NULL) {
		key = strtok(linebuf, ":");
		value = strtok(NULL, ":");
		if (key != NULL && value != NULL) {
			trim(key);
			trim(value);
			if ((strcmp(key, "Uid") == 0) && strstr(value, uidstr) != NULL) {
				// printf("[%s] UID=%s\n", filename, value);
				keepme = 1;
			}
			if (strcmp(key, "Name") == 0) {
				// printf("[%s] NAME=%s\n", filename, value);
				strcpy(procname, value);
			}
			if (strcmp(key, "Pid") == 0) {
				strcpy(pid, value);
			}
		}
	}
	if (keepme) {
		printf("%s: %s\n", pid, procname);
	}
	return 0;
}


int main(int argc, char **argv)
{
	uid_t uid;
	DIR *dirp;
	struct dirent *directory_entry;
	char dirname[256];

	/* validate the input and map name to uid */
	if (argc != 2) {
		print_usage(argc, argv);
		return 1;
	}
	uid = user_id_from_name(argv[1]);
	if (uid == -1) {
		printf("Invalid username specified (not found)\n");
		return 1;
	}

	/* now, walk directories in /proc fs */
	if ((dirp = opendir(PROCFS_ROOT)) == NULL) {
		perror("Unabled to open /proc");
		return 1;
	}
	do {
		if ((directory_entry = readdir(dirp)) != NULL) {
			if (directory_entry->d_type == DT_DIR ||
			    directory_entry->d_type == DT_LNK) {
				strcpy(&dirname[0], PROCFS_ROOT);
				strcat(&dirname[0], "/");
				strcat(&dirname[0], directory_entry->d_name);
				check_dir(&dirname[0], uid);
			}
		}
	} while (directory_entry != NULL);
	return 0;
}
