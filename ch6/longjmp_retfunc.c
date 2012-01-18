/*
 * Write a program to see what happens if we try to longjmp() into a
 * function that has already returned.
 */

#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

int func(int doJmpSwitch)
{
	if (doJmpSwitch) {
		switch (setjmp(env)) {
		case 0:
			printf("case 0\n");
			break;
		case 1:
			printf("case 1\n");
			break;
		case 2:
			printf("case 2\n");
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	func(1);
	func(0);
	longjmp(env, 0);
}
