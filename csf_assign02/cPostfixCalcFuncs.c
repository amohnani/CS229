/*
 * C postfix calculator
 * function implementation
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

/*
 * Example function (see tests in cTests.c)
 */
long addPositive(long a, long b) {
	if (a < 0L || b < 0L) {
		printf("Error: values must be positive\n");
		exit(1);
	}
	return a + b;
}

// Determine whether character code c is a space or tab
long isSpace(long c){
  if ((char)c == ' ' || (char)c == '\t'){
    return 1L;
  }
  return 0L;
}

// Determine whether character code c is a digit ('0'-'9')
long isDigit(long c){
  if (c > 47L && c < 58L){
    return 1L;
  }
  return 0L;
}

// Skip any leading space characters in the string s, returning a
// pointer to the first non-whitespace character (or the NUL terminator
// if there are no remaining non-whitespace characters)
const char *skipws(const char *s){
  int counter = 0;
  int length = strlen(s);
  while (counter < length && isSpace(s[counter])){
    counter++;
  }
  return (s + counter);
}

// Return the type (integer literal, +, -, *, or /) of the next
// token in string s
long tokenType(const char *s){
  if (isDigit(s[0])){
    return 0L;
  }else if (s[0] == '+'){
    return 1L;
  }else if (s[0] == '-'){
    return 2L;
  }else if (s[0] == '*'){
    return 3L;
  }else if (s[0] == '/'){
    return 4L;
  }
  return -1;
}

// Parse an integer literal, storing the integer value in the
// variable pointed to by pVal; returns pointer to first character
// in the string past the integer
const char *consumeInt(const char *s, long *pVal){
  int counter = 0;
  long temp = 0;
  long x = *s;
  while (counter < strlen(s) && isDigit(x)){
    temp *= 10;
    temp += x - 48;
    counter++;
    x = *(s + counter);
  }
  *pVal = temp;
  return s + counter;
}

// Push integer value onto the stack (variable pCount points to
// is the number of values on the stack
void stackPush(long stack[], long *pCount, long val){
  stack[*pCount] = val;
  (*pCount)++;
}

// Pop integer value from the stack (variable pCount points to
// is the number of values on the stack)
long stackPop(long stack[], long *pCount){
  (*pCount)--;
  return stack[*pCount];
}

// Evaluate given postfix expression, returning the result of
// the evaluation
long eval(const char *expr){
  long stack[20];
  long pCount = 0;
  long pVal = 0;

  //  long intsover = 0;
  //continues while string still has more characters
  while (strlen(expr) > 0){
    //removes whitespace and checks is string still has more characters
    expr = skipws(expr);
    if (strlen(expr) <= 0){
      break;
    }
    
    int token = tokenType(expr);
    //case of integer
    if (token == 0){
      /*  if (intsover){
	printf("Error: Invalid Expression.\n");
	exit(1);
	}*/
      if (pCount == 20){
	printf("Error: Operand stack is too large.\n");
	exit(1);
      }
      //pushes next value onto stack
      expr = consumeInt(expr, &pVal);
      stackPush(stack, &pCount, pVal);
    }else if (token > 0){
      //intsover = 1;
      if (pCount == 0 || pCount == 1){
	printf("Error: Not enough operands on stack.\n");
	exit(1);
      }
      //pops two values from stack
      int a = stackPop(stack, &pCount);
      int b = stackPop(stack, &pCount);
      //checks the different operators
      if (token == 1){
	stackPush(stack, &pCount, a + b);
      }else if (token == 2){
		stackPush(stack, &pCount, b - a);
      }else if (token == 3){
		stackPush(stack, &pCount, a * b);
      }else if (token == 4){
		stackPush(stack, &pCount, b/a);
      }
      expr = expr + 1;
      //case of invalid character
    }else{
      printf("Error: String contains an invalid character.\n");
      exit(1);
    }
  }
  //confirms only one value is left on stack
  if (pCount != 1){
    printf("Error: Invalid Expression.\n");
    exit(1);
  }
  return stackPop(stack, &pCount);
}

