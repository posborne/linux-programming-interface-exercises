#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


static void fpathconfPrint(const char *msg, int fd, int name)
{
        long lim;
        errno = 0;
        lim = fpathconf(fd, name);
        if (lim != -1) {
                printf("%s %ld\n", msg, lim);
        } else {
                if (errno == 0)
                        printf("%s (indeterminate)\n", msg);
                else
                        return;
        }
}


int main(int argc, char *argv[])
{
        printf("== STDIN ==\n");
        fpathconfPrint("_PC_NAME_MAX: ", STDIN_FILENO, _PC_NAME_MAX);
        fpathconfPrint("_PC_PATH_MAX: ", STDIN_FILENO, _PC_PATH_MAX);
        fpathconfPrint("_PC_PIPE_BUF: ", STDIN_FILENO, _PC_PIPE_BUF);

        printf("\n== ext4 ==\n");
        int fd = open("t_fpathconf.c", O_RDONLY);
        fpathconfPrint("_PC_NAME_MAX: ", fd, _PC_NAME_MAX);
        fpathconfPrint("_PC_PATH_MAX: ", fd, _PC_PATH_MAX);
        fpathconfPrint("_PC_PIPE_BUF: ", fd, _PC_PIPE_BUF);
        close(fd);

        return 0;
}
