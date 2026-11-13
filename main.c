#include <stdio.h>
#include "main.h"

int main(void) {
	char *s;
	size_t n;
	getline(&s, &n, stdin);
	exprp x = make_list_strf(s);
	return 0;
}
