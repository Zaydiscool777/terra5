#include "main.h"

struct expr nil = {ATOM, 0};

exprp append(exprp x, exprp y) {
	if (null(*x)) {
		return y;
	} else {
		exprp z = append(x->c.dr, y);
		return cons(x->c.ar, z);
	}
}

exprp pair(exprp x, exprp y) {
	if (null(*x) && null(*y)) {
		return &nil;
	} else if (!x->tag && !y->tag) {
		return cons(cons(x->c.ar, y->c.ar), pair(x->c.dr, y->c.dr));
	}
}

exprp assoc(exprp x, exprp y) { // y = ((a . b) (c . d) ...)
	if (eq(x, y->c.ar->c.ar)) {
		return y->c.ar->c.dr;
	} else {
		return assoc(x, y->c.dr);
	}
}

exprp eval(exprp e, exprp a);

exprp eval(exprp e, exprp a) {
	if (e->tag) {
		return assoc(e, a);
	} else if (e->c.ar->tag) {
		if (strncmp(e->c.ar->a, "quote", 6) == 0) {
			return e->c.dr->c.ar;
		} else if (strncmp(e->c.ar->a, "atom", 5) == 0) {
			return make_atom_i(eval(e->c.dr->c.ar, a)->tag);
		} else if (strncmp(e->c.ar->a, "eq", 3) == 0) {
			return make_atom_i(eq(eval(e->c.dr->c.ar, a), eval(e->c.dr->c.dr->c.ar, a)));
		} else if (strncmp(e->c.ar->a, "car", 4) == 0) {
			return eval(e->c.dr->c.ar, a)->c.ar;
		} else if (strncmp(e->c.ar->a, "cdr", 4) == 0) {
			return eval(e->c.dr->c.ar, a)->c.dr;
		} else if (strncmp(e->c.ar->a, "cons", 5) == 0) {
			return cons(eval(e->c.dr->c.ar, a), eval(e->c.dr->c.dr->c.ar, a));
		} else if (strncmp(e->c.ar->a, "cond", 5) == 0) {
			exprp evcon(exprp c) { // ((a . b) (c . d) ...)
				if (eval(c->c.ar->c.ar, a)->a) {
					return eval(c->c.ar->c.dr, a);
				} else {
					return evcon(c->c.dr);
				}
			}
			return evcon(e->c.dr);
		} else {
			return eval(cons(assoc(e->c.ar, a), e->c.dr), a);
				// mutates car e to assoc (car e) a
		}
	} else if (strncmp(e->c.ar->c.ar->a, "label", 6)) {
		return eval(cons(e->c.ar->c.dr->c.dr->c.ar, e->c.dr),
			// mutates car e to caddar e
			cons(cons(e->c.ar->c.dr->c.ar, e->c.ar), a));
	} else if (strncmp(e->c.ar->c.ar->a, "lambda", 7)) {
		exprp evlis(exprp m) {
			if (null(*m)) {
				return &nil;
			} else {
				return cons(eval(m->c.ar, a), evlis(m->c.dr));
			}
		}
		return eval(e->c.ar->c.dr->c.dr->c.ar,
			append(pair(e->c.ar->c.dr->c.ar,
				evlis(e->c.dr)), a));
	}
}
