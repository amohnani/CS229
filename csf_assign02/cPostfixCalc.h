/*
 * Common header file for C version of postfix calculator
 */

#ifndef CPOSTFIXCALC_H
#define CPOSTFIXCALC_H

/* TODO: add #defines for constants used by your functions */

long addPositive(long a, long b); /* example function */

// Determine whether character code c is a space or tab
long isSpace(long c);
// Determine whether character code c is a digit ('0'-'9')
long isDigit(long c);
// Skip any leading space characters in the string s, returning a
// pointer to the first non-whitespace character (or the NUL terminator
// if there are no remaining non-whitespace characters)
const char *skipws(const char *s);
// Return the type (integer literal, +, -, *, or /) of the next
// token in string s
long tokenType(const char *s);
// Parse an integer literal, storing the integer value in the
// variable pointed to by pVal; returns pointer to first character
// in the string past the integer
const char *consumeInt(const char *s, long *pVal);
// Push integer value onto the stack (variable pCount points to
// is the number of values on the stack)
void stackPush(long stack[], long *pCount, long val);
// Pop integer value from the stack (variable pCount points to
// is the number of values on the stack)
long stackPop(long stack[], long *pCount);
// Evaluate given postfix expression, returning the result of
// the evaluation
long eval(const char *expr);

#endif /* CPOSTFIXCALC_H */
