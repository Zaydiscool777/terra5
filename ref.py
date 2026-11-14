debug = 0  # 0.1
strict = False

if debug >= 0:
    import time

    global h, i
    h = 0
    i = 0


class Exp:
    nil = Exception()

    def __init__(self, a, b=None):
        self.atom = b is None
        if b is None:
            self.val = a
        else:
            self.car = a
            self.cdr = b

    def __eq__(self, x):
        return self.atom and x.atom and self.val == x.val

    def __bool__(self):
        return self != nil  # self.atom and self.val == 't'

    def __str__(self, inlist=False):
        if self.atom:
            if self == nil:
                return "()"
            else:
                return str(self.val)
        elif inlist:
            if self.cdr.atom:
                if self.cdr == nil:
                    return str(self.car)
                else:
                    return f"{self.car} . {self.cdr}"
            else:
                return f"{self.car} {self.cdr.__str__(True)}"
        elif self.cdr.atom:
            if self.cdr == nil:
                return f"({self.car})"
            else:
                return f"({self.car} . {self.cdr})"
        else:
            return f"({self.car} {self.cdr.__str__(True)})"

    __repr__ = __str__
    # def __repr__(self):
    # return f'Exp({repr(self.val)})' if self.atom else 'Expcons'


Exp.nil = Exp(None)
nil = Exp.nil


def lex(x):
    return (
        x.replace("(", "( ")
        .replace(")", " )")
        .replace("(  )", "( )")
        .replace("'", "' ")
        # .replace("#\n", "").replace("#\t", "").replace("#	", "")
        # .replace("\n", "").replace("\t", "").replace("#", "")
        .split(" " if strict else None)
    )


def tocons(x, dot=True):
    if len(x) == 0:  # ()
        return nil
    elif len(x) == 1:  # (a)
        return Exp(x[0], nil)
    if dot and x[-2] == Exp("."):  # (... a . b)
        i = Exp(x[-3], x[-1])
        x = x[:-3]
    else:  # (... a)
        i = Exp(x.pop(), nil)
    while len(x) > 0:
        i = Exp(x.pop(), i)
    return i


def parse(x):  # (res, cont)
    if len(x) == 0:
        raise Exception("(?")
    if x[0] == "(":
        cont = x[1:]
        a = []
        while cont[0] != ")":
            b, cont = parse(cont)
            a.append(b)
        return (tocons(a), cont[1:])
    elif len(x) == 0:
        return (nil, [])

    elif x[0] == "'":
        b, cont = parse(x[1:])
        return (Exp(Exp("quote"), Exp(b, nil)), cont)

    elif x[0] == ")":
        raise Exception(")")
    else:
        return (Exp(x[0]), x[1:])


def parsemain(x):
    x = lex(x)
    return parse(x)[0]
    a = []
    while len(x) > 0:
        b, x = parse(x)
        a.append(b)
    return tocons(a)


def append_(x, y):
    if x == nil:
        return y
    else:
        return Exp(x.car, append_(x.cdr, y))


def pair(x, y):
    if x == nil:
        return nil
    elif x.atom:
        return Exp(Exp(x, y), nil)  # two -> dot
    else:
        return Exp(Exp(x.car, y.car), pair(x.cdr, y.cdr))  # two -> dot


def assoc(x, y):
    if y.atom:
        return y
    elif x == y.car.car:
        return y.car.cdr  # - .car
    else:
        return assoc(x, y.cdr)


def in_(x, y):
    if y.atom:
        return False
    elif x == y.car.car:
        return True
    else:
        return in_(x, y.cdr)


def tolist(x):
    if x.atom:
        if x == nil:
            return []
        else:
            return [x]
    else:
        return tolist(x.car) + tolist(x.cdr)


def decdeb(f):
    if debug >= 0:
        return f
    else:

        def dummy(f):
            return f

        return dummy


@decdeb
def evil_dec(f):
    def ret(e, a):
        global h, i
        print("  " * (h + i) + ">", e)
        time.sleep(debug)
        h += 1
        x = f(e, a)
        h -= 1
        print("  " * (h + i) + "<", x)
        time.sleep(debug)
        return x

    return ret


@evil_dec
def evil(e, a):
    if e.atom:
        if e == nil:
            return nil
        else:
            return assoc(e, a)
    elif e.car.atom:
        match e.car.val:
            case "quote":
                return e.cdr.car
            case "atom":
                return evil(e.cdr.car, a).atom
            case "eq":
                return evil(e.cdr.car, a) == evil(e.cdr.cdr.car, a)
            case "car":
                return evil(e.cdr.car, a).car
            case "cdr":
                return evil(e.cdr.car, a).cdr
            case "cons":
                return Exp(evil(e.cdr.car, a), evil(e.cdr.cdr.car, a))
            case "cond":
                return evcon(e.cdr, a)

            case "let":  # (let (a b) c) -> c with a<->b
                e2 = e.cdr.cdr.car
                a2 = append_(pair(e.cdr.car.car, e.cdr.car.cdr.car), a)
                return evil(e2, a2)
            case "eval":
                return evil(evil(e.cdr.car, a), evil(e.cdr.cdr.car, a))
            case "dict":
                return a
            case None:
                return nil
            case "explode":
                return tocons(list(map(Exp, str(evil(e.cdr.car, a)))))
            case "ins":
                return Exp(lex(input()))
            case "outs":
                x = evil(e.cdr.car, a)
                print(*tolist(x), sep="", end="")
                return x
            case "wip":
                raise Exception("wip")
            case "!":
                input()
                x = evil(e.cdr, a)
                input()
                return x
            case "what":
                return assoc(e.cdr.car, a)

            case _:
                return evil(Exp(assoc(e.car, a), e.cdr), a)
    elif e.car.car.val == "label":  # ((label x f) i)
        e2 = Exp(e.car.cdr.cdr.car, e.cdr)
        a2 = Exp(Exp(e.car.cdr.car, e.car), a)  # two -> dot
        return evil(e2, a2)
    elif e.car.car.val == "lambda":  # ((lambda a f) i)
        a2 = append_(pair(e.car.cdr.car, evlis(e.cdr, a)), a)
        return evil(e.car.cdr.cdr.car, a2)
    elif e.car.car.val == "def":  # ((def f ar) ...) -> ((label f (lambda ar)) ...)
        return evil(
            Exp(
                tocons(
                    [Exp("label"), e.car.cdr.car, Exp(Exp("lambda"), e.car.cdr.cdr)]
                ),
                e.cdr,
            ),
            a,
        )


# @evcon_dec
def evcon(c, a):
    if c.atom:
        return evil(c, a)
    elif evil(c.car.car, a):
        return evil(c.car.cdr, a)
    else:
        return evcon(c.cdr, a)


def evlis(m, a):
    if m == nil:
        return nil
    else:
        return Exp(evil(m.car, a), evlis(m.cdr, a))


# ((lambda (a . b) b) (quote 1) (quote 2) (quote 3))
# (label sub (lambda (x y z) (cond ((atom z) (cond ((eq z y) x) ((quote t) z))) ((quote t) (cons (sub x y (car z)) (sub x y (cdr z))))))) ?
dict_ = tocons(
    [
        Exp(Exp("-"), parsemain("(lambda (x) (eq x ()))")),
        Exp(
            Exp("sub"),
            parsemain(
                "(lambda (x y z) (cond ((atom z) . (cond ((eq z y) . x) . z)) ('t . (cons (sub x y (car z)) (sub x y (cdr z))))))"
            ),
        ),
        Exp(
            Exp("~"),
            parsemain(
                "(lambda (x y) (cond ((- x) . y) ('t . (cons (car x) (~ (cdr x) y)))))"
            ),
        ),
        Exp(
            Exp("*"),
            parsemain(
                "(lambda (x y) (cond ((- x)) ((atom x) . (cons x y)) ((- y)) ('t . (cons (cons (car x) (car y)) (* (cdr x) (cdr y))))))"
            ),
        ),
        Exp(
            Exp(":"),
            parsemain(
                "(lambda (x y) (cond ((atom y) . y) ((eq x (car (car y))) . (cdr (car y))) ('t . (: x (cdr y)))))"
            ),
        ),
        Exp(
            Exp("evil"),
            parsemain("""
(lambda (e a) 
	(cond 
		((atom e) . (: e a)) 
		((atom (car e)) .  (cond 
			((eq (car e) 'quote) . (car (cdr e))) 
			((eq (car e) 'atom) . (atom (evil (car (cdr e)) a))) 
			((eq (car e) 'eq) . (eq (evil (car (cdr e)) a) (evil (car (cdr (cdr e))) a))) 
			((eq (car e) 'car) . (car (evil (car (cdr e)) a))) 
			((eq (car e) 'cdr) . (cdr (evil (car (cdr e)) a))) 
			((eq (car e) 'cons) . (cons (evil (car (cdr e)) a) (evil (car (cdr (cdr e))) a))) 
			((eq (car e) 'cond) . (evcon (cdr e) a)) 
			((eq (car e) 'let) . (evil (car (cdr (cdr e))) (~ (* (car (car (cdr e))) (car (cdr (car (cdr e))))) a))) 
			((- (car e))) 
			((eq (car e) '!) . (! evil (cdr e) a))
			((eq (car e) 'dict) . a)
			((eq (car e) 'eval) . (evil (evil (car (cdr e)) a) (evil (car (cdr (cdr e))) a)))
			((eq (car e) 'wip) . wip)
			('t . (evil (cons (: (car e) a) (cdr e)) a)) 
		)) 
		((eq (car (car e)) 'label) . (evil (cons (car (cdr (cdr (car e)))) (cdr e)) (cons (cons (car (cdr (car e))) (car e)) a))) 
		((eq (car (car e)) 'lambda) . (evil (car (cdr (cdr (car e)))) (~ (* (car (cdr (car e))) (evlis (cdr e) a)) a))) 
		((eq (car (car e)) 'def) . (evil (cons (cons 'lambda (cdr (cdr (car e)))) (cdr e)) 
			(cons (cons (car (cdr (car e))) (cons (cons 'lambda (cdr (cdr (car e)))) (cdr e))) a))) 
	) 
) 
"""),
        ),
        # (let ((f 1 2 r = lf l1 l2 lr) ((car e) (car (cdr e)) (car (cdr (cdr e))) (cdr e) (lambda (x) (eq f x))
        # 	(car (car e)) (car (cdr (car e))) (car (cdr (cdr (car e)))) (cdr (car e))))
        Exp(
            Exp("evcon"),
            parsemain(
                "(lambda (c a) (cond ((atom c) . (evil c a)) ((evil (car (car c)) a) . (evil (cdr (car c)) a)) ('t . (evcon (cdr c) a))))"
            ),
        ),
        Exp(
            Exp("evlis"),
            parsemain(
                "(lambda (m a) (cond ((- m)) ('t . (cons (evil (car m) a) (evlis (cdr m) a)))))"
            ),
        ),
    ]
)
test = "(eval '(: 'evil (dict)) (dict))"
print("ret", evil(parsemain(input()), dict_))
