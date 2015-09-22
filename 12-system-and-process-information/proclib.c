#include "proclib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* In-place time of string, s, to remove whitespace */
void trim(char * s) {
	char * p = s;
	int l = strlen(p);

	while(isspace(p[l - 1])) p[--l] = 0;
	while(* p && isspace(* p)) ++p, --l;

	memmove(s, p, l + 1);
}
