/*
 * Tests for postfix calculator
 */

#include <stdlib.h>
#include <string.h>
#include "tctest.h"

/*
 * Prototypes for assembly functions defined in asmPostfixCalc.S
 */
long addLongs(long a, long b); /* example assembly function */
/* TODO: add prototypes for your assembly language functions */

/* TODO: add #defines for constants used by your assembly language functions */

/*
 * Test fixture data type
 */
typedef struct {
	/*
	 * TODO: add fields for data needed by test functions
	 * (for example, a representation of the operand stack)
	 */
} TestObjs;

/*
 * Test fixture setup/cleanup functions: these
 * are called automatically as part of executing each
 * test function.
 */
TestObjs *setup(void);
void cleanup(TestObjs *objs);

/* test functions */
void testAddLongs(TestObjs *objs); /* example test function */
/* TODO: add prototypes for your test functions */

/* set to nonzero if a call to exit is expected */
int expectedExit;
/* jump buffer that our version of exit can use to jump back to test function */
sigjmp_buf exitBuf;

/*
 * Custom version of exit: useful for testing functions where
 * the expected behavior is a call to exit (e.g., because
 * an argument value is invalid.)  If the expectedExit
 * variable is set to a nonzero value, uses siglongjmp (via
 * exitBuf) to return control to the test function.
 * If expectedExit is not set, immediately fails the
 * current test.
 */
void exit(int exitCode) {
	if (expectedExit) {
		/* jump back to test function */
		siglongjmp(exitBuf, 1);
	} else {
		/* exit called unexpectedly, fail the test */
		FAIL("Unexpected exit");
	}
}

int main(void) {
	TEST_INIT();

	TEST(testAddLongs);
	/* TODO: use the TEST macro to execute each of your test functions */

	TEST_FINI();
}

TestObjs *setup(void) {
	/* reset expectedExit */
	expectedExit = 0;

	TestObjs *objs = malloc(sizeof(TestObjs));
	/* TODO: initialize fields of test fixture as necessary */
	return objs;
}

void cleanup(TestObjs *objs) {
	free(objs);
}

/*
 * Example test function
 */
void testAddLongs(TestObjs *objs) {
	ASSERT(2L == addLongs(1L, 1L));
	ASSERT(11L == addLongs(9L, 2L));
	ASSERT(-3L == addLongs(-14L, 11L));
}
