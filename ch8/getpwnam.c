/*
 * Implement getpwnam() using setpwent(), getpwent(), and endpwent().
 * 
 * Copyright 2012, Paul Osborne <osbpau@gmail.com>
 */
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

/*
 * Implementation of getpwnam using getpwent() and friends.
 * getpwent() gives us one entry at a time from the passwd file.
 *
 * The getpwnam() function searches the user database for an entry
 * with a matching name.
 *
 * The getpwnam() function returns a pointer to a struct passwd with
 * the structure as defined in <pwd.h> with a matching entry if
 * found. A null pointer is returned if the requested entry is not
 * found, or an error occurs. On error, errno is set to indicate the
 * error.
 *
 * The return value may point to a static area which is overwritten by a
 * subsequent call to getpwent(), getpwnam() or getpwuid().
 */
struct passwd * po_getpwnam(const char *name)
{
        struct passwd *pwd;
        while ((pwd = getpwent()) != NULL) {
                if (strcmp(name, pwd->pw_name) == 0) {
                        return pwd;
                }
        }
        return NULL;
}

void pprint_pwd(struct passwd *pwd)
{
        if (!pwd) {
                printf("passwd is NULL, not found\n");
                return;
        }
        printf("struct passwd {\n");
        printf("  pw_name=\"%s\",\n", pwd->pw_name);
        printf("  pw_uid=%d,\n", pwd->pw_uid);
        printf("  pw_gid=%d,\n", pwd->pw_gid);
        printf("  pw_dir=\"%s\",\n", pwd->pw_dir);
        printf("  pw_shell=\"%s\"\n", pwd->pw_shell);
        printf("}\n");
}

int main(int argc, char **argv)
{
        if (argc != 2) {
                printf("Usage: %s <name>\n", argv[0]);
                return 1;
        }
        pprint_pwd(po_getpwnam(argv[1]));
        return 0;
}
