#include "main.h"

struct expr *expr_copy(struct expr *o) {
	if (o == NULL) {
		return NULL;
	}

	struct expr *ret = malloc(sizeof(struct expr));
	ret->ref = -1;
	ret->tag = o->tag;
	if (ret->tag) {
		ret->a = malloc(sizeof(void *));
		*ret->a = *o->a;
	} else {
		ret->c.car = expr_copy(o->c.car);
		ret->c.cdr = expr_copy(o->c.cdr);
	}
	return ret;
}

struct list *list_copy(struct list* o) {
	struct list *ret = malloc(sizeof(struct list));
	ret->head = NULL;
	ret->then = &ret->head;

	for (struct expr *ato = o->head; ato != *o->then; ato = ato->c.cdr) {
		struct expr *node = malloc(sizeof(struct expr));
		node->tag = 0;
		node->ref = -1;
		node->c.car = expr_copy(ato->c.car);
		node->c.cdr = NULL;
		*ret->then = node;
		ret->then = &node->c.cdr;
	}

	return ret;
}
