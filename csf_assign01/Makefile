#
# Makefile for CSF Assignment 1
#
# You should not need to change anything in this makefile
#

C_SRCS = apintTests.c apintTests2.c apint.c tctest.c
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

%.o : %.c
	gcc $(CFLAGS) -c $<

all : apintTests

apintTests : apintTests.o apint.o tctest.o
	gcc -o $@ apintTests.o apint.o tctest.o

# Use this target to create a zipfile that you can submit to Gradescope
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile *.h *.c

clean :
	rm -f *.o apintTests depend.mak solution.zip

depend.mak :
	touch $@

depend :
	gcc -M $(C_SRCS) > depend.mak

include depend.mak
