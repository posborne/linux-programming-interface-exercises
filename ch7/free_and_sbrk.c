/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                Copyright (C) Paul Osborne, 2012.                     *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING for details.                      *
\**********************************************************************/

/* free_and_sbrk.c

   Test if free(3) actually lowers the program break.

   Usage: free_and_sbrk num-allocs block-size [step [min [max]]]

   Try: free_and_sbrk 1000 10240 2 1 1000
        free_and_sbrk 1000 10240 1 1 999
        free_and_sbrk 1000 10240 1 1 500

        (Only the last of these should see the program break lowered.)
   
  This is a modified version of the program to work with fewer
  dependencies.

*/
#define _BSD_SOURCE

#define MAX_ALLOCS 1000000
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

    printf("\n");

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        printf("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    numAllocs = atoi(argv[1]);
    if (numAllocs > MAX_ALLOCS) {
        printf("num-allocs > %d\n", MAX_ALLOCS);
        exit(EXIT_FAILURE);
    }

    blockSize = atoi(argv[2]);
    freeStep = (argc > 3) ? atoi(argv[3]) : 1;
    freeMin =  (argc > 4) ? atoi(argv[4]) : 1;
    freeMax =  (argc > 5) ? atoi(argv[5]) : numAllocs;

    if (freeMax > numAllocs) {
        printf("free-max > num-allocs\n");
        exit(EXIT_FAILURE);
    }

    printf("Initial program break:          %10p\n", (void *)sbrk(0));

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) {
        ptr[j] = malloc(blockSize);
        if (ptr[j] == NULL) {
            printf("malloc");
            exit(EXIT_SUCCESS);
        }
    }

    printf("Program break is now:           %10p\n", (void *)sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",
                freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep)
        free(ptr[j]);

    printf("After free(), program break is: %10p\n", (void *)sbrk(0));

    exit(EXIT_SUCCESS);
}
