// Test goto in the same CaseBlock, label after case label
// This should print "PASSED".

var v = 0;

function nop() { /* nothing */ }

function test(n)
{
	switch (n) {
	case 0:
		v += 1;
		goto L2;
	case 1:
		v += 10;
	case 2:
		nop();
	L2:
		v += 100;
	}
}

test(0);
print(v == 101 ? "PASSED" : "FAILED");
