/* t_stat.c

   A program that displays the information returned by the stat()/lstat()
   system calls.

   Usage: t_stat [-l] file

   The '-l' option indicates that lstat() rather than stat() should be used.
*/
#define _BSD_SOURCE   /* Get major() and minor() from <sys/types.h> */
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static void
fmttime(char *buf, struct tm *timeinfo, long nsec) {
  char tmp[20];
  strftime(buf, 80, "%a %b %d %H:%M:%S", timeinfo);
  sprintf(tmp, ".%09ld", nsec);
  strcat(buf, tmp);
  strftime(tmp, 80, " %Y\n", timeinfo);
  strcat(buf, tmp);
}


static void
displayStatInfo(const struct stat *sb)
{
    char buf[1024];
    printf("File type:                ");

    switch (sb->st_mode & S_IFMT) {
    case S_IFREG:  printf("regular file\n");            break;
    case S_IFDIR:  printf("directory\n");               break;
    case S_IFCHR:  printf("character device\n");        break;
    case S_IFBLK:  printf("block device\n");            break;
    case S_IFLNK:  printf("symbolic (soft) link\n");    break;
    case S_IFIFO:  printf("FIFO or pipe\n");            break;
    case S_IFSOCK: printf("socket\n");                  break;
    default:       printf("unknown file type?\n");      break;
    }

    printf("Device containing i-node: major=%ld   minor=%ld\n",
                (long) major(sb->st_dev), (long) minor(sb->st_dev));

    printf("I-node number:            %ld\n", (long) sb->st_ino);

    printf("Mode:                     %lo\n",
	   (unsigned long) sb->st_mode);

    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
        printf("    special bits set:     %s%s%s\n",
                (sb->st_mode & S_ISUID) ? "set-UID " : "",
                (sb->st_mode & S_ISGID) ? "set-GID " : "",
                (sb->st_mode & S_ISVTX) ? "sticky " : "");

    printf("Number of (hard) links:   %ld\n", (long) sb->st_nlink);

    printf("Ownership:                UID=%ld   GID=%ld\n",
            (long) sb->st_uid, (long) sb->st_gid);

    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
        printf("Device number (st_rdev):  major=%ld; minor=%ld\n",
                (long) major(sb->st_rdev), (long) minor(sb->st_rdev));

    printf("File size:                %lld bytes\n", (long long) sb->st_size);
    printf("Optimal I/O block size:   %ld bytes\n", (long) sb->st_blksize);
    printf("512B blocks allocated:    %lld\n", (long long) sb->st_blocks);

    fmttime(buf, localtime(&(*sb).st_atime), sb->st_atim.tv_nsec);
    printf("Last file access:         %s", buf);
    fmttime(buf, localtime(&(*sb).st_mtime), sb->st_mtim.tv_nsec);
    printf("Last file modification:   %s", buf);
    fmttime(buf, localtime(&(*sb).st_ctime), sb->st_ctim.tv_nsec);
    printf("Last status change:       %s", buf);
}

int
main(int argc, char *argv[])
{
    struct stat sb;
    int statLink;          /* True if "-l" specified (i.e., use lstat) */
    int fname;             /* Location of filename argument in argv[] */

    statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
                                /* Simple parsing for "-l" */
    fname = statLink ? 2 : 1;

    if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
        printf("%s [-l] file\n"
	       "        -l = use lstat() instead of stat()\n", argv[0]);
	return 1;
    }

    if (statLink) {
      if (lstat(argv[fname], &sb) == -1) {
	  printf("error: lstat\n");
	  return 1;
      }
    } else {
      if (stat(argv[fname], &sb) == -1) {
	printf("error: stat\n");
	return 1;
      }
    }

    displayStatInfo(&sb);

    return 0;
}
