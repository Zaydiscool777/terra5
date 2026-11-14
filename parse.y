%{
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

exprp parse_result;

static exprp make_nil(void);
int yylex(void);
void yyerror(const char *message);
%}

%union {
	exprp expr;
	char *atom;
}

%token <atom> SYMBOL
%token LPAREN RPAREN DOT QUOTE
%type <expr> object list
%start input

%%

input:
	object { parse_result = $1; }
	;

object:
	SYMBOL {
		$$ = make_atom_chars($1);
		free($1);
	}
	| LPAREN list RPAREN { $$ = $2; }
	| QUOTE object {
		$$ = cons(make_atom_chars("quote"), cons($2, make_nil()));
	}
	;

list:
	%empty { $$ = make_nil(); }
	| object list { $$ = cons($1, $2); }
	| object DOT object { $$ = cons($1, $3); }
	;

%%

static exprp make_nil(void) {
	exprp result = expr_alloc();
	result->tag = ATOM;
	memset(result->a, 0, sizeof(result->a));
	return result;
}

int yylex(void) {
	int c;
	while ((c = getchar()) != EOF && isspace((unsigned char)c)) {
	}

	switch (c) {
	case EOF:
		return 0;
	case '(':
		return LPAREN;
	case ')':
		return RPAREN;
	case '\'':
		return QUOTE;
	default:
		break;
	}

	size_t capacity = 32;
	size_t length = 0;
	char *text = malloc(capacity);
	if (text == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	do {
		if (length + 1 >= capacity) {
			capacity *= 2;
			char *resized = realloc(text, capacity);
			if (resized == NULL) {
				free(text);
				perror("realloc");
				exit(EXIT_FAILURE);
			}
			text = resized;
		}
		text[length++] = (char)c;
		c = getchar();
	} while (c != EOF && !isspace((unsigned char)c) &&
	         c != '(' && c != ')' && c != '\'');

	if (c != EOF) {
		ungetc(c, stdin);
	}
	text[length] = '\0';

	if (strcmp(text, ".") == 0) {
		free(text);
		return DOT;
	}

	yylval.atom = text;
	return SYMBOL;
}

void yyerror(const char *message) {
	fprintf(stderr, "parse error: %s\n", message);
}
