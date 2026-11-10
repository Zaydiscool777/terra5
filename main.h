#include <stddef.h>
#include <stdlib.h>

typedef void *atom;
struct expr;

typedef struct cell {
	struct expr *car;
	struct expr *cdr;
} cell;

struct expr {
	char tag; // 0 = cell, 1 = atom
	union {
		struct cell c;
		atom *a;
	};
	int ref;
};

struct list {
	struct expr *head;
	struct expr **then; // points to cell.cdr
};

struct expr *expr_copy(struct expr *o);
struct list *list_copy(struct list* o);
