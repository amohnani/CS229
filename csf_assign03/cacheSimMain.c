#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cacheSimFuncs.h"


int main(int argc, char **argv) 
{
  //checks for correct number of command line arguments
  if (argc != 7) {
    printf("Invalid input.\n");
    return 1;
  }
  Cache *cache = malloc(sizeof(Cache));
  //creates cache with given parameters
  if (defineCache(cache, argv) != 0) {
    printf("Invalid input.\n");
    free(cache);
    return 1;
  }

  //loops through trace file
  if (loopThroughFile(cache) != 0) {
    destroy(cache);
    return 1;
  }

  //displays results and frees memory
  displayResults(cache);
  destroy(cache);
  return 0; 
} 
