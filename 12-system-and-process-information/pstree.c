/*
Write a program that draws a tree showing the hierarchical
parent-child relationships of all processes on the system, going all
the way back to init.  For each process, the program should display
the process ID and the command being executed.  The output of the
program should be similar to that produced by pstree(1), although it
does need not to be as sophisticated.  The parent of each process on
the system can be found by inspecing the PPid: line of all of the
/proc/PID/status files on the system.  Be careful to handle the
possibilty that a process's parent (and thus its /proc/PID directory)
disappears during the scan of all /proc/PID directories.

---

Providing this output consists of two main steps:

  1) Build the process tree based on information from the proc filesystem.
  2) Render the tree using a recursion (pre-order traversal)

*/
#include "proclib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>

#define PROCFS_ROOT "/proc"

/* acts as node in both linked list and tree */
struct pstree_node {
	char name[128];
	pid_t pid;
	pid_t ppid;
	struct pstree_node * parent;
	struct pstree_node * children[128];
	struct pstree_node * next;
};

static struct pstree_node * pstree_root = NULL;
static struct pstree_node * ll_head;

int ll_create_and_insert(char *procname, pid_t pid, pid_t ppid)
{
	struct pstree_node *node;
	node = (struct pstree_node*)malloc(sizeof(struct pstree_node));
	if (node == NULL) {
		printf("Unable to allocate memory for node\n");
		return 1;
	}
	strcpy(node->name, procname);
	node->pid = pid;
	node->ppid = ppid;
	node->children[0] = NULL;
	node->parent = NULL;
	node->next = ll_head; /* could be null, that is fine */
	ll_head = node;
	return 0;
}


int catalog_process(char *dirname)
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
			} else if (strcmp(key, "PPid") == 0) {
				strcpy(ppid, value);
			} else if (strcmp(key, "Name") == 0) {
				strcpy(procname, value);
			}
		}
	}

	return ll_create_and_insert(&procname[0], atoi(pid), atoi(ppid));
}

struct pstree_node * find_node_with_pid(pid_t pid)
{
	struct pstree_node *node;
	for (node = ll_head; node != NULL; node = node->next) {
		if (node->pid == pid) {
			return node;
		}
	}
	return NULL;
}

int make_tree(void)
{
	int i;
	struct pstree_node *node, *pnode;

	/* now we have a valid linked list, make a tree */
	for (node = ll_head; node != NULL; node = node->next) {
		i = 0;
		pnode = find_node_with_pid(node->ppid);
		if (pnode != NULL) {
			node->parent = pnode;
			while (pnode->children[i++] != NULL);
			pnode->children[i - 1] = node;
			pnode->children[i] = NULL;
		}
	}
	return 0;
}

int print_tree(struct pstree_node * root, int level)
{
	int i;
	struct pstree_node *node;
	for (i = 0; i < level; i++) {
		printf("  ");
	}
	printf("%s (pid: %d, ppid: %d)\n", root->name, root->pid, root->ppid);
	/* recurse on children */
	while ((node = root->children[i++]) != NULL) {
		print_tree(node, level + 1);
	}
	return 0;
}

int main(int argc, char **argv)
{
	DIR *dirp;
	struct dirent *directory_entry;
	char dirname[256];

	/* now, walk directories in /proc fs to create nodes */
	if ((dirp = opendir(PROCFS_ROOT)) == NULL) {
		perror("Unabled to open /proc");
		return 1;
	}
	do {
		if ((directory_entry = readdir(dirp)) != NULL) {
			if (directory_entry->d_type == DT_DIR) {
				strcpy(&dirname[0], PROCFS_ROOT);
				strcat(&dirname[0], "/");
				strcat(&dirname[0], directory_entry->d_name);
				catalog_process(dirname);
			}
		}
	} while (directory_entry != NULL);

	/* turn the list into a tree */
	make_tree();

	/* print the tree */
	struct pstree_node *node;
	for (node = ll_head; node != NULL; node = node->next) {
		// printf("node: %s, pid: %d, ppid: %d\n", node->name, node->pid, node->ppid);
		if (node->parent == NULL) {
			print_tree(node, 0);
		}
	}

	return 0;
}
