#include "main.h"
#include <glib-2.0/glib.h>

GList *arena = NULL;
exprp expr_alloc(void) {
	exprp x = g_malloc(sizeof(struct expr));
	arena = g_list_prepend(arena, x);
	return x;
}
inline void arena_clear(void) {
	g_list_free_full(arena, g_free);
}

exprp expr_copy(exprp o) {
	if (o == NULL) {
		return NULL;
	}

	exprp ret = expr_alloc();
	ret->tag = o->tag;

	switch (o->tag) {
		case CELL:
			ret->c.ar = expr_copy(o->c.ar);
			ret->c.dr = expr_copy(o->c.dr);
			ret->c.ref = UNDEF;
			break;
		case ATOM:
			memcpy(&ret->a, &o->a, sizeof(atom));
			break;
	}

	return ret;
}

void expr_free(exprp r) {
	if (r == NULL) {
		return;
	}

	switch (r->tag) {
		case CELL:
			expr_free(r->c.ar);
			expr_free(r->c.dr);
		default: break;
	}

	free(r);
}

inline void nilify(exprp x) {
	memset(&x->a, 0, sizeof(atom));
}

exprp make_atom_chars(char *x) {
	exprp e = expr_alloc();
	e->tag = ATOM;
	strncpy(e->a, x, (sizeof(atom) - 1));
	return e;
}

exprp cons(exprp a, exprp d) {
	exprp ret = expr_alloc();
	ret->tag = CELL;
	ret->c.ar = a;
	ret->c.dr = d;
	return ret;
}

bool eq(exprp x, exprp y) {
	if (x->tag && y->tag) {
		return memcmp(x->a, y->a, sizeof(atom)) == 0;
	} else {
		return false;
	}
}

// https://stackoverflow.com/q/1872156
bool null(struct expr x) {
	for (int i = 0; i < sizeof(atom); i++) {
		if (x.a[i] != '0') {
			return false;
		}
	}
	return true;
}

exprp make_atom_p(void *x) {
	exprp ret = expr_alloc();
	ret->tag = ATOM;
	memcpy(&ret->a, x, sizeof(atom));
	return ret;
}

exprp make_atom_i(int x) {
	exprp ret = expr_alloc();
	ret->tag = ATOM;
	memcpy(&ret->a, &x, sizeof(int));
	return ret;
}

exprp make_list_str1(char *x) {
	exprp at = expr_alloc();
	nilify(at);
	exprp ret = at;
	if (strlen(x) > 0) {
		char s[2] = {x[0], '\0'};
		at->c.ar = make_atom_chars(s);
		nilify(at->c.dr);
	}
	for (int i = 1; i < strlen(x); i++) {
		at->c.dr = expr_alloc();
		at = at->c.dr;
		char s[2] = {x[i], '\0'};
		at->c.ar = make_atom_chars(s);
		nilify(at->c.dr);
	}
	return ret;
}

exprp make_list_strf(char *x) {
	exprp at = expr_alloc();
	nilify(at);
	exprp ret = at;
	if (strlen(x) > 0) {
		at->c.ar = make_atom_chars(x);
		nilify(at->c.dr);
	}
	for (int i = sizeof(atom) - 1; i < strlen(x); i += sizeof(atom) - 1) {
		at->c.dr = expr_alloc();
		at = at->c.dr;
		at->c.ar = make_atom_chars(x + i);
		nilify(at->c.dr);
	}
	return ret;
}
