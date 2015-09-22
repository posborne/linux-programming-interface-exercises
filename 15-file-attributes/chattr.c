/*
Name
----
chattr - change file attributes on a Linux file system

Synopsis
--------
chattr [ mode ] files...

Description
-----------
chattr changes the file attributes on a Linux file system.  The format
of a symbolic mode is +-=[acdeijstuADST].

The operator '+' causes the selected attributes to be added to the
existing attributes of the files; '-' causes them to be removed; and
'=' causes them to be the only attributes that the files have.

The letters 'acdeijstuADST' select the new attributes for the files:
append only (a), compressed (c), no dump (d), extent format (e),
immutable (i), data journalling (j), secure deletion (s), no
tail-merging (t), undeletable (u), no atime updates (A), synchronous
directory updates (D), synchronous updates (S), and top of directory
hierarchy (T).

The following attributes are read-only, and may be listed by lsattr(1)
but not modified by chattr: huge file (h), compression error (E),
indexed directory (I), compression raw access (X), and compressed
dirty file (Z).

Attributes
----------
When a file with the 'A' attribute set is accessed, its atime record
is not modified. This avoids a certain amount of disk I/O for laptop
systems.  A file with the 'a' attribute set can only be open in append
mode for writing. Only the superuser or a process possessing the
CAP_LINUX_IMMUTABLE capability can set or clear this attribute.

A file with the 'c' attribute set is automatically compressed on the
disk by the kernel. A read from this file returns uncompressed data. A
write to this file compresses data before storing them on the
disk. Note: please make sure to read the bugs and limitations section
at the end of this document.

When a directory with the 'D' attribute set is modified, the changes
are written synchronously on the disk; this is equivalent to the
'dirsync' mount option applied to a subset of the files.

A file with the 'd' attribute set is not candidate for backup when the
dump(8) program is run.

The 'E' attribute is used by the experimental compression patches to
indicate that a compressed file has a compression error. It may not be
set or reset using chattr(1), although it can be displayed by
lsattr(1).

The 'e' attribute indicates that the file is using extents for mapping
the blocks on disk. It may not be removed using chattr(1).

The 'I' attribute is used by the htree code to indicate that a
directory is being indexed using hashed trees. It may not be set or
reset using chattr(1), although it can be displayed by lsattr(1).

The 'h' attribute indicates the file is storing its blocks in units of
the filesystem blocksize instead of in units of sectors, and means
that the file is (or at one time was) larger than 2TB. It may not be
set or reset using chattr(1), although it can be displayed by
lsattr(1).

A file with the 'i' attribute cannot be modified: it cannot be deleted
or renamed, no link can be created to this file and no data can be
written to the file. Only the superuser or a process possessing the
CAP_LINUX_IMMUTABLE capability can set or clear this attribute.

A file with the 'j' attribute has all of its data written to the ext3
journal before being written to the file itself, if the filesystem is
mounted with the "data=ordered" or "data=writeback" options. When the
filesystem is mounted with the "data=journal" option all file data is
already journalled and this attribute has no effect. Only the
superuser or a process possessing the CAP_SYS_RESOURCE capability can
set or clear this attribute.

When a file with the 's' attribute set is deleted, its blocks are
zeroed and written back to the disk. Note: please make sure to read
the bugs and limitations section at the end of this document.

When a file with the 'S' attribute set is modified, the changes are
written synchronously on the disk; this is equivalent to the 'sync'
mount option applied to a subset of the files.

A directory with the 'T' attribute will be deemed to be the top of
directory hierarchies for the purposes of the Orlov block
allocator. This is a hint to the block allocator used by ext3 and ext4
that the subdirectories under this directory are not related, and thus
should be spread apart for allocation purposes. For example it is a
very good idea to set the 'T' attribute on the /home directory, so
that /home/john and /home/mary are placed into separate block
groups. For directories where this attribute is not set, the Orlov
block allocator will try to group subdirectories closer together where
possible.

A file with the 't' attribute will not have a partial block fragment
at the end of the file merged with other files (for those filesystems
which support tail-merging). This is necessary for applications such
as LILO which read the filesystem directly, and which don't understand
tail-merged files. Note: As of this writing, the ext2 or ext3
filesystems do not (yet, except in very experimental patches) support
tail-merging.

When a file with the 'u' attribute set is deleted, its contents are
saved. This allows the user to ask for its undeletion. Note: please
make sure to read the bugs and limitations section at the end of this
document.

The 'X' attribute is used by the experimental compression patches to
indicate that a raw contents of a compressed file can be accessed
directly. It currently may not be set or reset using chattr(1),
although it can be displayed by lsattr(1).

The 'Z' attribute is used by the experimental compression patches to
indicate a compressed file is dirty. It may not be set or reset using
chattr(1), although it can be displayed by lsattr(1).

*/
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fs.h>

#define ACTION_ADD     '+'
#define ACTION_REMOVE  '-'
#define ACTION_ONLY    '='

#define ATTRS_LIST                      "acdeijstuADT"
#define ATTR_APPEND_ONLY                'a'
#define ATTR_COMPRESSED                 'c'
#define ATTR_NO_DUMP                    'd'
#define ATTR_EXTENT_FORMAT              'e'
#define ATTR_IMMUTABLE                  'i'
#define ATTR_JOURNALLING                'j'
#define ATTR_SECURE_DELETE              's'
#define ATTR_NO_TAIL_MERGE              't'
#define ATTR_UNDELETABLE                'u'
#define ATTR_NO_ATIME_UPDATES           'A'
#define ATTR_SYNCHRONOUS_DIR_UPDATES    'D'
#define ATTR_TOP_OF_DIRECTORY_HIERARCHY 'T'

#define LIST_LENGTH(x) ((sizeof(x) / sizeof(x[0])))

struct attr_table_entry {
	char code;
	int fs_flag;
};

static struct attr_table_entry fs_attrs[] = {
	{ATTR_APPEND_ONLY, FS_APPEND_FL},
	{ATTR_COMPRESSED, FS_COMPR_FL},
	{ATTR_NO_DUMP, FS_NODUMP_FL},
	{ATTR_EXTENT_FORMAT, FS_EXTENT_FL},
	{ATTR_IMMUTABLE, FS_IMMUTABLE_FL},
	{ATTR_JOURNALLING, FS_JOURNAL_DATA_FL},
	{ATTR_SECURE_DELETE, FS_SECRM_FL},
	{ATTR_NO_TAIL_MERGE, FS_NOTAIL_FL},
	{ATTR_UNDELETABLE, FS_UNRM_FL},
	{ATTR_NO_ATIME_UPDATES, FS_NOATIME_FL},
	{ATTR_SYNCHRONOUS_DIR_UPDATES, FS_DIRSYNC_FL},
	{ATTR_TOP_OF_DIRECTORY_HIERARCHY, FS_TOPDIR_FL},
};


static void
print_usage()
{
	fprintf(stderr, "Usage: ./prog_chattr [mode] files\n");
	exit(EXIT_FAILURE);
}


static struct attr_table_entry*
lookup_attr_table_entry(char attr)
{
	struct attr_table_entry *entry;
	unsigned long i;
	for (i = 0; i < LIST_LENGTH(fs_attrs); i++) {
		entry = &fs_attrs[i];
		if (entry->code == attr) {
			return entry;
		}
	}
	return NULL;
}


static bool
is_valid_attr(char attr)
{
	char c;
	int i = 0;
	while ((c = ATTRS_LIST[i++]) != '\0') {
		if (c == attr) {
			return true;
		}
	}
	return false;
}


static void
validate_mode_string(char *mode_string) {
	int len;
	char attr;
	int i;
	len = strlen(mode_string);
	if (len < 2) {
		fprintf(stderr, "ERROR: Invalid mode string specified (too short)\n");
		print_usage();
	}
	i = 1;
	while ((attr = mode_string[i++]) != '\0') {
		if (!is_valid_attr(attr)) {
			fprintf(stderr, "ERROR: Attribute '%c' is invalid\n", attr);
			print_usage();
		}
	}
}


static int
get_mask_for_attrs(char *attrs)
{
	int mask;
	int i;
	char attr;
	struct attr_table_entry *entry;

	i = 0;
	mask = 0;
	while ((attr = attrs[i++]) != '\0') {
		entry = lookup_attr_table_entry(attr);
		if (entry == NULL) {
			/* we should have already validated */
			fprintf(stderr, "ERROR: Invalid Attr '%c'\n", attr);
			exit(EXIT_FAILURE);
		}
		mask |= (entry->fs_flag);
	}
	return mask;
}


static int
transform_attrs_add(int attrs, int mask)
{
	return (attrs | mask);
}


static int
transform_attrs_remove(int attrs, int mask)
{
	/* 
	 * To ensure that the new attrs does not have any of the bits
	 * from mask set, we take the inverse of the mask and and it
	 * with the current attrs
	 */
	int reversed_mask;
	int new_attrs;
	reversed_mask = ~mask;
	new_attrs = (attrs & reversed_mask);
	return new_attrs;
}


static int
transform_attrs_only(int attrs, int mask)
{
	return mask; /* In this case, just use the mask */
}


static int
do_action(char *attrs, int file_count, char **files, int(*transform_attrs)(int, int))
{
	int mask;
	int i;
	size_t fd;
	char *filename;
	int current_attrs;
	int new_attrs;

	/* get mask for our attributes */
	mask = get_mask_for_attrs(attrs);

	/* Do the update on each file */
	for (i = 0; i < file_count; i++) {
		filename = files[i];
		fd = open(filename, 0);
		if (fd < 0) {
			fprintf(stderr, "ERROR: Unable to open %s, skipping\n", filename);
			continue;
		}

		/* Get the current flags */
		if (ioctl(fd, FS_IOC_GETFLAGS, &current_attrs) == -1) {
			fprintf(stderr, "ERROR: Unable to get flags on %s, skipping\n", filename);
			goto cleanup;
		}
		printf("cur attrs: 0x%08x, mask: 0x%08X\n", current_attrs, mask);
		new_attrs = transform_attrs(current_attrs, mask); /* enable all flags in mask */
		printf("new attrs: 0x%08X\n", new_attrs);
		if (ioctl(fd, FS_IOC_SETFLAGS, &new_attrs) == -1) {
			fprintf(stderr, "ERROR: Unable to set flags on %s, skipping\n", filename);
		}
cleanup:
		close(fd);
	}
	return 0;
}


int main(int argc, char **argv)
{
	int file_count = 0;
	char *mode_string;
	char *attrs;
	char action;
	if (argc < 3) {
		print_usage();
	}
	mode_string = argv[1];
	file_count = argc - 2;
	validate_mode_string(mode_string);
	action = mode_string[0];
	attrs = &mode_string[1];
	switch (action) {
	case ACTION_ADD:
		return do_action(attrs, file_count, &argv[2], transform_attrs_add);
		break;
	case ACTION_REMOVE:
		return do_action(attrs, file_count, &argv[2], transform_attrs_remove);
		break;
        case ACTION_ONLY:
		return do_action(attrs, file_count, &argv[2], transform_attrs_only);
		break;
	}
	return 0;
}
