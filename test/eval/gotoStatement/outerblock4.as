// Test forward goto out of a 'try' block past a 'finally'
// This should print "PASSED".

var v = 0;

function test()
{
	try {
		v = 1;
		goto L1;
		v = 2;
	}
	catch (e) {
		print("DEAD CODE #1, SHOULD NOT RUN");
	}
	finally {
		v += 100;
	}
	print("DEAD CODE #2, SHOULD NOT RUN");
L1: return;
}

test();
print(v == 101 ? "PASSED" : "FAILED");
