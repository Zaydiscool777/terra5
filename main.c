#include "main.h"
#include "eval.c"
#include <stdio.h>

int main(void) {
	yyparse();
	expr_show(*eval(parse_result, &nil), false);
	puts("");
}
