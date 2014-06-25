/*
 * Verify that calls to stat() do not change any of a file's
 * timestamps.
 */
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  struct stat initial_stat;
  struct stat post_stat_stat;
  stat("exercise2_stattest.c", &initial_stat);
  stat("exercise2_stattest.c", &post_stat_stat);

  printf("== Initial Stat ==\n");
  printf("st_atime: %d\n", (int)initial_stat.st_atime);
  printf("st_mtime: %d\n", (int)initial_stat.st_mtime);
  printf("st_ctime: %d\n", (int)initial_stat.st_ctime);

  printf("\n... Waiting 2 seconds ...\n\n");
  sleep(2);

  printf("== After First stat() call ==\n");
  printf("st_atime: %d\n", (int)post_stat_stat.st_atime);
  printf("st_mtime: %d\n", (int)post_stat_stat.st_mtime);
  printf("st_ctime: %d\n", (int)post_stat_stat.st_ctime);

  return 0;
}
