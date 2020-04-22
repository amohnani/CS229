#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cacheSimFuncs.h"

//This function handles a load command given the address
void load(Cache *cache, unsigned long hex) {
  cache->loads++;
  //finds the tag and index
  unsigned long tag = getTag(cache, hex);
  unsigned long index = getIndex(cache, hex);

  //checks for a hit
  if (checkForHit(cache, tag, index)) {
    cache->loadHits++;
    cache->cycles++;
    loadHit(cache, tag, index);
    
    //in the case of a miss
  } else {
    cache->loadMisses++;
    cache->cycles += 1 + (cache->numBytes/4)*100;
    loadMiss(cache, tag, index);
  }
}

//handles a store command given a memory address
void store(Cache *cache, unsigned long hex) {
  cache->stores++;
  //gets the tag and index
  unsigned long tag = getTag(cache, hex);
  unsigned long	index =	getIndex(cache, hex);

  //checks if the block exists already in the cache
  if (checkForHit(cache, tag, index)) {
    cache->storeHits++;
    if (cache->writeThrough == 0) {
      cache->cycles += 100;
    } else {
      cache->cycles += 1;
    }
    storeHit(cache, tag, index);
    //case of a miss
  } else {
    cache->storeMisses++;
    storeMiss(cache, tag, index);
  }
}

//function that handles the case of a store miss
void storeMiss(Cache *cache, unsigned long tag, unsigned long index) {
  //in the case of write-allocate
  if (cache->writeAlloc == 0) {
    cache->cycles += (cache->numBytes / 4) * 100;
    //case of write-through
    if (cache->writeThrough == 0) {
      loadMiss(cache, tag, index);
      cache->cycles += 100;
      //case of write-back
    } else {
      cache->valid[loadMiss(cache, tag, index)] = -1;
      cache->cycles++;
    }
    //in the case of no-write-allocate
  } else {
    cache->cycles += 100;
  }
}

//Function that handles a load miss
int loadMiss(Cache *cache, unsigned long tag, unsigned long index) {
  int maxIndex = index; //index of new element
  if (cache->eviction == 1) { //case for fifo

    //iterates through blocks in set
    for (unsigned long i = index*cache->numBlocks; i < index*cache->numBlocks + cache->numBlocks; i++){
      //finds block to be evicted
      if (cache->metadata[i] == cache->numBlocks - 1) {
	maxIndex = i;
	break;
      }
    }

    

  } else { //case for lru

    //finds block to be evicted
    for (unsigned long i = index*cache->numBlocks; i < index*cache->numBlocks + cache->numBlocks; i++){
      //finds empty blocks
      if (cache->valid[i] == 0) {
	maxIndex = i;
	break;
      }
      //finds max order if all blocks are full
      if (cache->metadata[i] > cache->metadata[maxIndex]) {
	maxIndex = i;
      }
    }
    
  }
  //increments metadata
  for (unsigned long i = index * cache->numBlocks; i < index*cache->numBlocks + cache->numBlocks; i++){
    cache->metadata[i]++;
  }

  //evicts dirty blocks if necessary
  if (cache->valid[maxIndex] == -1) {
    cache->cycles += (cache->numBytes / 4) * 100;
  }

  //sets new block
  cache->metadata[maxIndex] = 0;
  cache->valid[maxIndex] = 1;
  cache->data[maxIndex] = tag;

  //returns index of new block
  return maxIndex;
}


//case of a Load Hit
void loadHit(Cache *cache, unsigned long tag, unsigned long index) {
  int location = getLocation(cache, tag, index);
  //finds order or recently added block
  int val = cache->metadata[location];

  //only in case of lru does metadata have to be updated
  if (cache->eviction == 0) {
    //iterates through blocks in set and increments other values
    for (unsigned long i = index * cache->numBlocks; i < index*cache->numBlocks+ cache->numBlocks; i++) {
      if (cache->metadata[i] <= val) {
	cache->metadata[i]++;
      }
    }
    //sets block at location to 0, since it is most recently used
    cache->metadata[location] = 0;
  }
}

//case of a store hit
void storeHit(Cache *cache, unsigned long tag, unsigned long index) {
  //marks blocks as dirty if necessary
  if (cache->writeThrough == 1) {
    cache->valid[getLocation(cache, tag, index)] = -1;
  }
  //follows same process as load hit
  loadHit(cache, tag, index);
}

//checks if a given tag has a hit
int checkForHit(Cache *cache, unsigned long tag, unsigned long index) {
  //loops through blocks in set
  for (unsigned long i = index * cache->numBlocks; i < index*cache->numBlocks + cache->numBlocks; i++) {
    //checks for tag
    if (tag == cache->data[i] && cache->valid[i]) {
      return 1;
    }
  }
  return 0;
}

//finds location of a tag given that the tag exists in the block
int getLocation(Cache *cache, unsigned long tag, unsigned long index) {
  //loops through blocks in set
  for (unsigned long i = index * cache->numBlocks; i < index*cache->numBlocks + cache->numBlocks; i++) {
    //returns index of tag
    if (tag == cache->data[i] && cache->valid[i]) {
      return i;
    }
  }
  return 0;
}

//gets tag from memory address
unsigned long getTag(Cache *cache, unsigned long hex) {
  //returns hex after shifting out offset and index
  return (hex >> (logtwo(cache->numBytes) + logtwo(cache->numSets)));
}

//gets index from memory address
unsigned long getIndex(Cache *cache, unsigned long hex) {
  hex = hex >> (logtwo(cache->numBytes));
  unsigned long temp = hex;
  temp >>= logtwo(cache->numSets);
  temp <<= logtwo(cache->numSets);
  return hex - temp;
}

//main loop that iterates through given trace file
int loopThroughFile(Cache *cache) {
  char command;
  unsigned long hex;
  char extra[10];

  //if there is more in file
  while (scanf(" %c", &command) == 1) {
    //case for load
    if (command == 'l') {
      scanf(" %lx", &hex);
      load(cache, hex);
      //case for store
    }else if (command == 's') {
      scanf(" %lx", &hex);
      store(cache, hex);
      //case for an invalid input
    }else {
      printf("Invalid trace file.\n");
      return 1;
    }
    scanf(" %s", extra);
  }
  return 0;
}

//defines a cache given command line inputs
int defineCache(Cache *cache, char **argv){
  cache->numSets = strtol(argv[1], NULL, 10);
  cache->numBlocks = strtol(argv[2], NULL, 10);
  cache->numBytes = strtol(argv[3], NULL, 10);

  //checks that all numbers are positive powers of 2
  if (!checkPositivePower(cache->numSets)
      || !checkPositivePower(cache->numBlocks)
      || !checkPositivePower(cache->numBytes)
      || cache->numBytes < 4) {
    return 1;
  }

  //determines write-allocate vs no-write-allocate
  if (strcmp(argv[4],"write-allocate") == 0) {
    cache->writeAlloc = 0;
  }else if (strcmp(argv[4],"no-write-allocate") == 0) {
    cache->writeAlloc = 1;
  }else {
    return 1;
  }

  //determines write-through vs write-back
  if (strcmp(argv[5],"write-through") == 0) {
    cache->writeThrough = 0;
  } else if (strcmp(argv[5], "write-back") == 0) {
    cache->writeThrough = 1;
  } else {
    return 1;
  }

  //determines lru vs fifo
  if (strcmp(argv[6], "lru") == 0) {
    cache->eviction = 0;
  } else if (strcmp(argv[6], "fifo") == 0) {
    cache->eviction = 1;
  } else {
    return 1;
  }

  //checks for case of no-write-allocate and write-back
  if (cache->writeAlloc == 1 && cache->writeThrough == 1) {
    printf("Cannot combine no-write-allocate and write-back.\n");
    return 1;
  }

  //iniates values of cache
  initiateCache(cache);
  
  return 0;
}

//checks if a number is a positive power of two
int checkPositivePower(int num) {
  if (num <= 0) {
    return 0;
  }
  int counter = 1;
  while (counter <= num) {
    if (counter == num) {
      return 1;
    }
    counter *= 2;
  }
  return 0;
}

//displays results at the end of a cache
void displayResults(Cache *cache) {
  printf("Total loads: %d\n", cache->loads);
  printf("Total stores: %d\n", cache->stores);
  printf("Load hits: %d\n", cache->loadHits);
  printf("Load misses: %d\n", cache->loadMisses);
  printf("Store hits: %d\n", cache->storeHits);
  printf("Store misses: %d\n", cache->storeMisses);
  printf("Total cycles: %d\n", cache->cycles);
}

//finds the log base 2 of a number
int logtwo(int num) {
  int counter = 1;
  int answer = 0;
  while (counter <= num) {
    if (counter == num) {
      return answer;
    }
    counter *= 2;
    answer++;
  }
  return 0;
}

//frees memory
void destroy(Cache *cache){
  free(cache->data);
  free(cache->valid);
  free(cache->metadata);
  free(cache);
}

//sets values of cache variables
void initiateCache(Cache *cache) {
  //sets all data values to 0
  cache->data = calloc(cache->numBlocks * cache->numSets, sizeof(long));
  cache->valid = calloc(cache->numBlocks * cache->numSets, sizeof(short));
  cache->metadata = calloc(cache->numBlocks * cache->numSets, sizeof(int));
  //sets values of orders in case of fifo
  if (cache->eviction == 1) {
    for (int i = 0 ; i < cache->numBlocks * cache->numSets; i++) {
      cache->metadata[i] = cache->numBlocks - (i % cache->numBlocks) - 1;
    }
  }

  //sets values of all other stuff to 0
  cache->cycles = 0;
  cache->loads = 0;
  cache->stores = 0;
  cache->loadHits = 0;
  cache->loadMisses = 0;
  cache->storeHits = 0;
  cache->storeMisses = 0;
}

//outputs cache for debugging purposes
void outputCache(Cache *cache) {
  printf("\n");
  for (int i = 0; i < cache->numBlocks * cache->numSets; i++) {
    printf("Tag: %x   Metadata: %d   Valid: %d\n", cache->data[i]
	   , cache->metadata[i], cache->valid[i]);
  }
  printf("\n");
}
