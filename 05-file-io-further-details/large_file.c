/* 
 * Modify the program in listing 5-3 to use standard file I/O system 
 * calls (open() and lseek()) and the off_t data type.  Compile the 
 * program with the _FILE_OFFSET_BITS macro set to 64, and test it to 
 * show that a large file can be successfully created. 
 */

//#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS (64)
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	off_t off;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		printf("%s pathname offset\n", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDWR | O_CREAT,
		    S_IRUSR | S_IWUSR);
	if (fd == -1) {
		printf("open\n");
		return 1;
	}
	
	off = atoll(argv[2]);
	if (lseek(fd, off, SEEK_SET) == -1) {
		printf("lseek\n");
		return 1;
	}
	
	if (write(fd, "text", 4) == -1) {
		printf("write\n");
		return 1;
	}
	printf("Success\n");
	return 0;
}
