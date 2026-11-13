#ifndef HEADER
#define HEADER
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

enum ref_t {
	UNDEF,
	ROOT,
	UNMARKED,
	MARKED,
};

enum tag_t {
	CELL,
	ATOM,
};

struct expr;
typedef struct expr *exprp;

struct cell {
	exprp ar;
	exprp dr;
	enum ref_t ref;
};

typedef char atom[sizeof(struct cell)]; // can store many things, even 23 chars

// any lisp expression. like numbers and stuff
struct expr {
	enum tag_t tag; // 0 = cell, 1 = atom
	union {
		struct cell c;
		atom a;
	};
};

// struct unit {} unit;

exprp expr_alloc(void);
exprp expr_copy(exprp o);
void expr_free(exprp r);
void nilify(exprp x);
exprp make_atom_chars(char *value);
exprp cons(exprp a, exprp d);
bool eq(exprp x, exprp y);
bool null(struct expr x);
exprp make_atom_p(void *x);
exprp make_atom_i(int x);
exprp make_list_str1(char *x);
exprp make_list_strf(char *x);

#endif
