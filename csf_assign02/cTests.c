/*
 * Unit tests for C version of postfix calculator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tctest.h"
#include "cPostfixCalc.h"

typedef struct {
	/* TODO: add fields for test fixture */
} TestObjs;

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));

	/* TODO: initialize test fixture fields */

	return objs;
}

void cleanup(TestObjs *objs) {
	/* TODO: do any cleanup needed for test fixture */

	free(objs);
}

/*
 * Prototypes for test functions
 */
void testAddPositive(TestObjs *obj);         /* example test function */
void testAddPositiveInvalid(TestObjs *obj);  /* example test function */
/* TODO: add prototypes for your test functions */
void testIsSpace(TestObjs *obj);
void testIsDigit(TestObjs *obj);
void testskipws(TestObjs *obj);
void testTokenType(TestObjs *obj);
void testConsumeInt(TestObjs *obj);
void testStackPushPop(TestObjs *obj);
void testEval(TestObjs *obj);


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

	TEST(testAddPositive);
	TEST(testAddPositiveInvalid);
	/* TODO: use the TEST macro to execute each of your test functions */
	TEST(testIsSpace);
	TEST(testIsDigit);
	TEST(testskipws);
	TEST(testTokenType);
	TEST(testConsumeInt);
	TEST(testEval);
	TEST(testStackPushPop);
	TEST_FINI();
}

void testTokenType(TestObjs *obj){
  ASSERT(tokenType("0++") == 0);
  ASSERT(tokenType("+ 0") == 1);
  ASSERT(tokenType("-0") == 2);
  ASSERT(tokenType("*") == 3);
  ASSERT(tokenType("/ 0") == 4);
  ASSERT(tokenType(" + 0") == -1);
}

void testConsumeInt(TestObjs *obj){
  const char *s = "2 3 4";
  long val;
  s = consumeInt(s, &val);
  ASSERT(val == 2);
  ASSERT(0 == strcmp(s, " 3 4"));
  s = "23 9 0";
  s = consumeInt(s, &val);
  ASSERT(val == 23);
  ASSERT(0 == strcmp(s, " 9 0"));
}

void testStackPushPop(TestObjs *obj){
  long stack[10];
  long pCount = 0;
  stackPush(stack, &pCount, 10L);
  ASSERT(10L == stackPop(stack, &pCount));
  stackPush(stack, &pCount, 20);
  stackPush(stack, &pCount, 40);
  stackPush(stack, &pCount, 30);
  ASSERT(stack[0] == 20);
  ASSERT(30 == stackPop(stack, &pCount));
  ASSERT(40 == stackPop(stack, &pCount));
  ASSERT(20 == stackPop(stack, &pCount));
}

void testEval(TestObjs *obj){
  
  char *c = "2 32 + ";
  long answer = eval(c);
  ASSERT(answer == 34L);

  c = "3 4 5 +*";
  answer = eval(c);
  ASSERT(answer == 27L);

  c = "7 2 -";
  answer = eval(c);
  ASSERT(answer == 5L);

  
  c = "2 3 4 5 +-* ";
  answer = eval(c);
  ASSERT(answer == -12L);
  
  expectedExit = 1;
  c = "10 2 -*";
  if (sigsetjmp(exitBuf, 1) == 0) {
    answer = eval(c);
    FAIL("With too many operators did not exit");
  } else {
    printf("Eval exited, good...");
  }
  
  c = "2 3 + 4";
  if (sigsetjmp(exitBuf, 1) == 0) {
    answer = eval(c);
    FAIL("With too many operands did not exit");
  } else {
    printf("Eval exited, good...");
  }

}


void testskipws(TestObjs *obj){
  char *temp = "     567";
  ASSERT(0 == strcmp("567", skipws(temp)));
  temp = "8";
  ASSERT(0 == strcmp("8", skipws(temp)));
  temp = "    ";
  ASSERT(0 == strcmp("",  skipws(temp)));
  temp = "    7";
  ASSERT(0 == strcmp("7", skipws(temp)));
}


void testIsDigit(TestObjs *obj){
  ASSERT(isDigit('0'));
  ASSERT(isDigit('9'));
  ASSERT(!isDigit(':'));
  ASSERT(!isDigit(' '));
}

void testIsSpace(TestObjs *obj){
  ASSERT(isSpace(' '));
  ASSERT(isSpace('\t'));
  ASSERT(!isSpace('f'));
  ASSERT(!isSpace('\n'));
}






/*
 * Example test function.
 */
void testAddPositive(TestObjs *obj) {
	ASSERT(2L == addPositive(1L, 1L));
	ASSERT(23L == addPositive(15L, 8L));
}

/*
 * Example of a test function that verifies that an invalid
 * input results in a call to the exit function.
 */
void testAddPositiveInvalid(TestObjs *obj) {
	/*
	 * addPositive should call exit if either of its arguments is negative
	 */

	expectedExit = 1;

	if (sigsetjmp(exitBuf, 1) == 0) {
		addPositive(-1L, 1L);
		FAIL("addPositive with invalid first argument did not exit");
	} else {
		printf("addPositive exited, good...");
	}

	if (sigsetjmp(exitBuf, 1) == 0) {
		addPositive(1L, -1L);
		FAIL("addPositive with invalid second argument did not exit");
	} else {
		printf("addPositive exited, good...");
	}
}

/* TODO: add your test functions */
