#include "main.h"
#include <string.h>

struct expr *expr_copy(struct expr *o) {
	if (o == NULL) {
		return NULL;
	}

	struct expr *ret = malloc(sizeof(struct expr));
	ret->ref = -1;
	ret->tag = o->tag;

	if (ret->tag) {
		memcpy(ret->a, o->a, sizeof(atom));
	} else {
		ret->c.car = expr_copy(o->c.car);
		ret->c.cdr = expr_copy(o->c.cdr);
	}

	return ret;
}

void expr_free(struct expr *r) {
	if (r == NULL) {
		return;
	}

	if (!r->tag) {
		expr_free(r->c.car);
		expr_free(r->c.cdr);
	}
	free(r);
}

struct list *list_copy(struct list* o) {
	struct list *ret = malloc(sizeof(struct list));
	ret->head = NULL;
	ret->then = &ret->head;

	for (struct expr *ato = o->head; ato != *o->then; ato = ato->c.cdr) {
		struct expr *this = malloc(sizeof(struct expr));
		this->tag = 0;
		this->ref = -1;
		this->c.car = expr_copy(ato->c.car);
		this->c.cdr = NULL;
		*ret->then = this;
		ret->then = &this->c.cdr;
	}

	return ret;
}

void list_free(struct list* r) {
	expr_free(r->head);
}

struct expr *make_atom_chars(char *value) {
	struct expr *e = malloc(sizeof(struct expr));
	e->tag = 1;
	strncpy(((char *)e->a), value, (sizeof(atom) - 1));
	e->ref = 0;
	return e;
}

// make_list_string
