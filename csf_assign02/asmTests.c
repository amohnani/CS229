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
long isSpace(long a);
long tokenType(const char *s);
long isDigit(long a);
const char *skipws(const char *s);
const char *consumeInt(const char *s, long *pVal);
void stackPush(long stack[], long *pCount, long val);
long stackPop(long stack[], long *pCount);
long eval(const char *c);

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
void testIsSpace(TestObjs *objs);
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

	TEST(testAddLongs);
	/* TODO: use the TEST macro to execute each of your test functions */
	TEST(testIsDigit);
	TEST(testIsSpace);
	TEST(testTokenType);
	TEST(testskipws);
	TEST(testConsumeInt);
	TEST(testStackPushPop);
	TEST(testEval);
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

  c = "8 2 /";
  answer = eval(c);
  ASSERT(answer == 4L);

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

void testskipws(TestObjs *obj){
  char *temp = "     567";
  ASSERT(0 == strcmp("567", skipws(temp)));
  temp = "    ";
  ASSERT(0 == strcmp("",  skipws(temp)));
  temp = "9";
  ASSERT(0 == strcmp("9", skipws(temp)));

  temp = "    7";
  ASSERT(0 == strcmp("7", skipws(temp)));
}


void testIsSpace(TestObjs *obj){
  ASSERT(isSpace(' '));
  ASSERT(isSpace('\t'));
  ASSERT(!isSpace('f'));
  ASSERT(!isSpace('\n'));
}


void testIsDigit(TestObjs *obj){
  ASSERT(isDigit('0'));
  ASSERT(isDigit('9'));
  ASSERT(!isDigit(':'));
  ASSERT(!isDigit(' '));
}

void testTokenType(TestObjs *obj){
  ASSERT(tokenType("0++") == 0);
  ASSERT(tokenType("+ 0") == 1);
  ASSERT(tokenType("-0") == 2);
  ASSERT(tokenType("*") == 3);
  ASSERT(tokenType("/ 0") == 4);
  ASSERT(tokenType(" + 0") == -1);
}

/*
 * Example test function
 */
void testAddLongs(TestObjs *objs) {
	ASSERT(2L == addLongs(1L, 1L));
	ASSERT(11L == addLongs(9L, 2L));
	ASSERT(-3L == addLongs(-14L, 11L));
}
