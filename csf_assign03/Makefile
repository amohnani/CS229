#
# CSF Assignment 3 Makefile
#

# Executables to build for the "all" target
EXES = csim 

# C compiler flags
CFLAGS = -g -Wall -Wextra --pedantic

# Default target: build all executables
all : csim


# Rule for compiling and assembling a .c file to produce a .o file
cacheSimMain.o : cacheSimMain.c
	gcc $(CFLAGS) -c cacheSimMain.c

#Rule for compiling cacheSimFuncs.o
cacheSimFuncs.o : cacheSimFuncs.c cacheSimFuncs.h
	gcc $(CFLAGS) -c cacheSimFuncs.c


# C version of postfix calculator
csim : cacheSimMain.o cacheSimFuncs.o
	gcc -o $@ $(LDFLAGS) cacheSimMain.o cacheSimFuncs.o

# This target creates a zipfile (solution.zip) that can be uploaded
# to Gradescope.
assign3.zip : Makefile cacheSimMain.c cacheSimFuncs.c
	rm -f $@
	zip -9r $@ Makefile README cacheSimFuncs.h cacheSimMain.c cacheSimFuncs.c

# Clean up generated files (such as object files and executables)
clean :
	rm -f *.o $(EXES) assign3.zip

