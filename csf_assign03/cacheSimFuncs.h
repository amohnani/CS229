#ifndef CACHESIMFUNCS_H
#define CACHESIMFUNCS_H

//declares struct Cache to hold variables
typedef struct cache_{
  int numSets;
  int numBlocks;
  int numBytes;
  int writeAlloc; //0 = write-allocate, 1 = no-write-allocate
  int writeThrough; //0 = write-through, 1 = write-back
  int eviction; //0 = lru, 1 = fifo

  unsigned int *data;
  int *metadata;
  short *valid;
  
  int loads;
  int stores;
  int loadHits;
  int loadMisses;
  int storeHits;
  int storeMisses;
  int cycles;
  
}Cache;

//declares functions that are described in .c file
int defineCache(Cache *cache, char **argc);
int checkPositivePower(int a);
int loopThroughFile(Cache *cache);
void load(Cache *cache, unsigned long hex);
void store(Cache *cache, unsigned long hex);
void displayResults(Cache *cache);
int logtwo(int num);
unsigned long getTag(Cache *cache, unsigned long hex);
unsigned long getIndex(Cache *cache, unsigned long hex);
void storeMiss(Cache *cache, unsigned long tag, unsigned long index);
int checkForHit(Cache *cache, unsigned long tag, unsigned long index);
int getLocation(Cache *cache, unsigned long tag, unsigned long index);
int loadMiss(Cache *cache, unsigned long tag, unsigned long index);
void destroy(Cache *cache);
void initiateCache(Cache *cache);
void outputCache(Cache *cache);
void loadHit(Cache *cache, unsigned long tag, unsigned long index);
void storeHit(Cache *cache, unsigned long tag, unsigned long index);

#endif 
