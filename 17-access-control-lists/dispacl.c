/*
 * Display ACL entry for some provided user or group.
 * 
 * Usage: ./prog_dispacl u <user> <file>
 * Usage: ./prog_dispacl g <group> <file>
 *
 * Write a program that displays the permissions from the ACL entry that
 * corresponds to a particular user or group.  The program should take
 * two command-line arguments.  The first argument is iether of the
 * letters u or g, indicating whether the second argument identifies a
 * user or group.  (The functions defined in Listing 8-1, on page 159, can
 * be used to allow the second command-line argument to be specified
 * numerically or as a name.)  If the ACL entry that corresponds to the
 * given user or group falls into the group class, then the program
 * should additionally display the permissions that would apply after
 * the ACL entry has been modified by the ACL mask entry.
 * 
 * -----
 *
 * Note: to build, you may need to install libacl.  On ubuntu 12.04,
 * this can be done by doing:
 *
 *   $ sudo apt-get install libacl1-dev
 */

#include <sys/acl.h>
#include <acl/libacl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *file = NULL;
char *user = NULL;
char *group = NULL;

void
print_usage(void)
{
	fprintf(stderr,
		"Usage:n"\
		" $ ./prog_dispacl u <user> <file>\n" \
		" $ ./prog_dispacl g <group> <file>\n");
	exit(EXIT_FAILURE);
}


void
parse_arguments(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "Expected 3 arguments\n");
		print_usage();
	}
	if (argv[1][0] == 'u' && argv[1][1] == '\0') {
		user = argv[2];
	} else if (argv[1][0] == 'g' && argv[1][1] == '\0') {
		group = argv[2];
	} else {
		print_usage();
	}
	file = argv[3];
}


int
do_display_acl(void)
{
	acl_t acl;
	acl_entry_t acl_entry;
	acl_tag_t acl_tag;
	int entry_id;

	acl = acl_get_file(file, ACL_TYPE_ACCESS);
	if (acl == NULL) {
		fprintf(stderr, "Unable to open ACL for file %s\n", file);
		exit(EXIT_FAILURE);
	}
	for (entry_id = ACL_FIRST_ENTRY;;entry_id = ACL_NEXT_ENTRY) {
		if (acl_get_entry(acl, entry_id, &acl_entry) != 1) {
			break; /* no more records or error */
		}
		if (acl_get_tag_type(acl_entry, &acl_tag) == -1) {
			fprintf(stderr, "Unexpected error on acl_get_tag_type\n");
			exit(EXIT_FAILURE);
		}
		printf("%-12s\n",
		       (acl_tag == ACL_USER_OBJ) ? "user_obj" :
		       (acl_tag == ACL_USER) ? "user" :
		       (acl_tag == ACL_GROUP_OBJ) ? "group_obj" :
		       (acl_tag == ACL_GROUP) ? "group" :
		       (acl_tag == ACL_MASK) ? "mask" :
		       (acl_tag == ACL_OTHER) ? "other" : "???");
	}

	if (acl_free(acl) == -1) {
		fprintf(stderr, "Unable to free ACL\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int
main(int argc, char **argv)
{
	parse_arguments(argc, argv);
	return do_display_acl();
}
