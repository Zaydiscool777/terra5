#include <stddef.h>
#include <stdlib.h>

struct expr;

typedef struct cell {
	struct expr *car;
	struct expr *cdr;
} cell;

typedef char atom[sizeof(cell)]; // can store many things, even 15 chars

struct expr {
	char tag; // 0 = cell, 1 = atom
	union {
		struct cell c;
		atom a;
	};
	int ref; // -1 = root
};

struct list {
	struct expr *head;
	struct expr **then; // points to cell.cdr
};

struct expr *expr_copy(struct expr *o);
struct list *list_copy(struct list* o);
