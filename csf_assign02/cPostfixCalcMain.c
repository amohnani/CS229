/*
 * C postfix calculator
 * main function
 */

#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Wrong number of arguments\n");
		return 1;
	}
	long answer = eval(argv[1]);
	if (answer != -1){
	  printf("Result is: %ld\n", answer);
	}else{
	  return 1;
	}
	return 0;
}
