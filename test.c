#include "main.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct expr *make_atom(char *value) {
	struct expr *e = malloc(sizeof(struct expr));
	e->tag = 1;
	e->a = malloc(sizeof(*e->a));
	*e->a = value;
	e->ref = 0;
	return e;
}

static void free_expr(struct expr *e) {
	if (e == NULL) {
		return;
	}

	if (e->tag) {
		free(e->a);
	} else {
		free_expr(e->c.car);
		free_expr(e->c.cdr);
	}
	free(e);
}

static void test_nonempty_list_copy(void) {
	char first[] = "first";
	char second[] = "second";
	struct expr *source_head = malloc(sizeof(struct expr));
	struct expr *source_tail = malloc(sizeof(struct expr));
	source_head->tag = 0;
	source_head->ref = 0;
	source_head->c.car = make_atom(first);
	source_head->c.cdr = source_tail;
	source_tail->tag = 0;
	source_tail->ref = 0;
	source_tail->c.car = make_atom(second);
	source_tail->c.cdr = NULL;

	struct list source = {source_head, &source_tail->c.cdr};
	struct list *copy = list_copy(&source);

	assert(copy != NULL);
	assert(copy != &source);
	assert(copy->head != source.head);
	assert(copy->then == &copy->head->c.cdr->c.cdr);
	assert(*copy->then == NULL);
	assert(strcmp((char *)*copy->head->c.car->a, first) == 0);
	assert(strcmp((char *)*copy->head->c.cdr->c.car->a, second) == 0);
	assert(copy->head->c.car != source.head->c.car);
	assert(copy->head->c.car->a != source.head->c.car->a);

	*copy->head->c.car->a = "changed";
	assert(strcmp((char *)*source.head->c.car->a, first) == 0);

	free_expr(source_head);
	free_expr(copy->head);
	free(copy);
}

static void test_empty_list_copy(void) {
	struct list source = {NULL, &source.head};
	struct list *copy = list_copy(&source);

	assert(copy != NULL);
	assert(copy->head == NULL);
	assert(copy->then == &copy->head);
	free(copy);
}

int main(void) {
	test_nonempty_list_copy();
	test_empty_list_copy();
	puts("list_copy tests passed");
	return 0;
}
