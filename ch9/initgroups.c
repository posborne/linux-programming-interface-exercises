#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/*
 * Description
 *
 * The initgroups() function initializes the group access list by
 * reading the group database /etc/group and using all groups of which
 * user is a member. The additional group group is also added to the
 * list.
 *
 * The user argument must be non-NULL.
 * 
 * Return Value
 *
 * The initgroups() function returns 0 on success. On error, -1 is
 * returned, and errno is set appropriately.  Errors
 *
 * - ENOMEM: Insufficient memory to allocate group information structure.
 * - EPERM:  The calling process has insufficient privilege. See the underlying
 *           system call setgroups(2).
 */
int po_initgroups(const char *user, gid_t group)
{
        struct group *grp;
        char *username;
        int i, groups_idx;
        gid_t groups[NGROUPS_MAX];
        groups_idx = 0;
        groups[groups_idx++] = group;
        while ((grp = getgrent()) != NULL) {
                i = 0;
                while ((username = grp->gr_mem[i++]) != NULL) {
                        if (strcmp(username, user) == 0) {
                                groups[groups_idx++] = grp->gr_gid;
                        }
                }
        }

        return setgroups(groups_idx, groups);
}


int main(int argc, char **argv)
{
        struct passwd *pwd;
        pwd = getpwnam(argv[1]);
        if (po_initgroups(argv[1], pwd->pw_gid) == -1) {
                perror("call to initgroups failed");
                return -1;
        } else {
                printf("Success!  I think...");
        }
        return 0;
}
