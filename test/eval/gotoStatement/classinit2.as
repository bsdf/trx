// This should compile because, while the var directive introduces an
// instance variable (arguably a language bug), the *labeled
// statement* is itself a part of the static initializer.  The body of
// that labeled statement should be executed only by the instance
// initializer.
//
// When run, it should print "5" twice: the initializer for x should
// only be executed by the constructor, because "var x = ..." is
// hoisted.

class C {
    static var iter = 0;
L0: goto L1;
L1: var x = (print(iter), iter);
    iter++;
    if (iter < 5) goto L0;
	print(iter);
};
(new C);
