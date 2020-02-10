/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "apint.h"

//creates an apint using one unint64_t value
ApInt *apint_create_from_u64(uint64_t val) {
	
  //creates a new apint with one array index
  ApInt *u64 = malloc(sizeof(ApInt));
  u64->nums = malloc(sizeof(uint64_t));
  u64->nums[0] = val;
  u64->capacity = 1;
  return u64;
}

//creates an apint using a variable length hexstring
ApInt *apint_create_from_hex(const char *hex) {
  //creates a new apint
  ApInt *fromHex = malloc(sizeof(ApInt));

  //copies string so that it can be modified
  int length = strlen(hex);
  char *hex2 = malloc(sizeof(char)*(length+1));
  strcpy(hex2,hex);

  //calculates capacity and allocates memory
  fromHex->capacity = (length/16) + (length % 16 != 0);
  fromHex->nums = malloc(sizeof(uint64_t)*fromHex->capacity);
  int counter = 0;

  //repeats while there are more than 16 hex digits
  while (length > 16){
    //adds last 16 digits to array
    fromHex->nums[counter] = (uint64_t) strtoul(hex2 + length - 16, NULL, 16);
    hex2[length - 16] = '\0';
    counter++;
    length -= 16;
  }

  //adds final part of hex string to array
  fromHex->nums[counter] = strtoul(hex2, NULL, 16);
  free(hex2);
	return fromHex;
}

//frees memory used by apint
void apint_destroy(ApInt *ap) {
  //frees memory used by array and by pointer to apint object
  free(ap->nums);
  free(ap);
}

//returns index n of array
uint64_t apint_get_bits(ApInt *ap, unsigned n) {
  //if index is too large
  if (n >= (unsigned)ap->capacity){
	return 0UL;
  }
  return ap->nums[n];
}

//finds 1 in the highest place value in bitstring
int apint_highest_bit_set(ApInt *ap) {
	/* TODO: implement */
  //loops through array
  for (int i = ap->capacity-1; i >= 0; i--){
    for (int j = 63; j >= 0; j--){
      //loops through 64 bits and checks if each bit is a 1
      if ((ap->nums[i] & (1UL << j)) != 0){
	return j+64*i;
      }
    }
  }
  //returns -1 if no 1s are found
	return -1;
}

//mimics left shift operation on integers
ApInt *apint_lshift(ApInt *ap) {
  return apint_lshift_n(ap, 1);
}

//allows for left shift multiple times
ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
  //creates new apint to be returned
    ApInt *apnew = malloc(sizeof(ApInt));
  apnew->capacity = ap->capacity;
  apnew->nums= malloc(sizeof(uint64_t)*ap->capacity);
  for (int i = 0; i < ap->capacity; i++){
    apnew->nums[i] = ap->nums[i];
  }

  //iterates for each time the apint has to be shifted
  for (int j = 0; j < (int) n; j++){
    // int highest = apint_highest_bit_set(apnew);
  //checks if another block has to be allocated
    if ((apnew->nums[ap->capacity-1] & (1UL << 63)) != 0){
      apnew->capacity++;
      apnew->nums = realloc(apnew->nums, sizeof(uint64_t)*apnew->capacity);
      apnew->nums[apnew->capacity-1] = 0UL;
    }

    //shifts the bits left
    for (int i = apnew->capacity - 1; i >= 1; i--){
      apnew->nums[i] = (apnew->nums[i] << 1);
      //checks if bits need to be shifted into the next array block
      if (i != 0 && (apnew->nums[i-1] & (1UL << 63)) != 0){
	apnew->nums[i]++;
      }
    }
    //shifts last uint64_t by 1
    apnew->nums[0] = (apnew->nums[0] << 1);
  }
  return apnew;
  
}

//returns an apint in the form of a hexstring
char *apint_format_as_hex(ApInt *ap) {

  //allocates memory for entire hex string and buffer
  char *hex = calloc(ap->capacity*18,sizeof(char));
  char *temp = calloc(17, sizeof(char));
  
  //loops through each uint64_t
  for (int i = ap->capacity - 1; i >= 0; i--){
    //adds hex string to buffer then concatenates to final string
    //if (ap->nums[i] == 0 && i < ap->capacity - 1){
    //sprintf(temp, "0000000000000000");
    //}else{
      sprintf(temp, "%lx", ap->nums[i]);
      //}
      if (i < ap->capacity -1){
	char *bigtemp = calloc(18, sizeof(char));
	for (int i = 0; i < (int)(16 - strlen(temp)); i++){
	  strcat(bigtemp, "0");
	}
	strcat(bigtemp, temp);
	strcpy(temp, bigtemp);
	free(bigtemp);
      }
    strcat(hex, temp);
  }
  //frees memory and returns hex string
  free(temp);
	return hex;
}

//adds two apints together
ApInt *apint_add(const ApInt *a, const ApInt *b) {
  //creates apint to be returned
  if (apint_compare(a, b) < 0){
    const ApInt *temp = a;
    a = b;
    b = temp;
  }
  ApInt *add = malloc(sizeof(ApInt));
  //sets capacity equal to larger of two capacities
  if (a->capacity >= b->capacity){
    add->nums = malloc(sizeof(uint64_t)*a->capacity);
    add->capacity = a->capacity;
  }else{
    add->nums = malloc(sizeof(uint64_t)*b->capacity);
    add->capacity = b->capacity;
  }

  //repeats through each array block
  for (int i = add->capacity-1; i >= 0; i--){
    //checks if both apints are valid for a given index
    if (i < a->capacity && i < b->capacity){
      
      //checks if there will be overflow
      if (a->nums[i] + b->nums[i] < a->nums[i]){
	//checks if a new block has to be allocated
	if (i == add->capacity - 1){
	  add->capacity++;
	  add->nums = realloc(add->nums, sizeof(uint64_t)*add->capacity);
	  add->nums[add->capacity - 1] = 0UL;
	}
	add->nums[i] = a->nums[i] + b->nums[i];
	add->nums[i+1]++;
	//adds through the blocks
	for (int j = i+1; j < add->capacity; j++){
	  if (add->nums[j] == 0UL){
	    if (j == add->capacity - 1){
	      add->capacity++;
	      add->nums = realloc(add->nums, sizeof(uint64_t)*add->capacity);
	      add->nums[add->capacity - 1] = 0UL;
	    }
	    add->nums[j+1]++;
	  }else{
	    break;
	  }
	}
	//case of no overflow
      }else{
	add->nums[i] = a->nums[i] + b->nums[i];
      }
      //if only one of the apints is valid at a given index
    }else if (a->capacity >= i){
      add->nums[i] = a->nums[i];
    }else if (b->capacity >= i){
      add->nums[i] = b->nums[i];
    }
  }
  return add;
}

//computes the difference of two apints
ApInt *apint_sub(const ApInt *a, const ApInt *b) {
  //confirms that a is larger than b
  if (apint_compare(a,b) < 0){
	return NULL;
  }

  //creates apint to be returned
  ApInt *sub = malloc(sizeof(ApInt));
  sub->nums = malloc(sizeof(uint64_t) * a->capacity);
  sub->capacity = a->capacity;

  //repeats through each index of array
  for (int i = a->capacity - 1; i >= 0; i--){
    if (i >=  b->capacity){
      sub->nums[i] = a->nums[i];
    }else{
      if (a->nums[i] >= b->nums[i]){
	sub->nums[i] = a->nums[i] - b->nums[i];
	//checks for borrowing
      }else{
	sub->nums[i] = 0xFFFFFFFFFFFFFFFFUL - b->nums[i] + a->nums[i] + 1;
	sub->nums[i+1] = sub->nums[i+1] - 1;
	//removes a block if necessary
	if (sub->nums[i+1] == 0 && (i+1) == sub->capacity - 1){
	  sub->capacity--;
	  sub->nums = realloc(sub->nums, sizeof(uint64_t) * sub->capacity);
	}
	for (int j = i + 1; j < sub->capacity; j++){
	  if (sub->nums[j] == 0xFFFFFFFFFFFFFFFFUL){
	    sub->nums[j+1]--;
	    if (sub->nums[j+1] == 0 && (j+1) == sub->capacity - 1){
	      sub->capacity--;
	      sub->nums = realloc(sub->nums, sizeof(uint64_t) * sub->capacity);
	    }
	  }else{
	    break;
	  }
	}
      }
      //again, removes a block if necessary
      if (sub->nums[i] == 0 && (i) == sub->capacity - 1 && sub->capacity > 1){
	  sub->capacity--;
	  sub->nums = realloc(sub->nums, sizeof(uint64_t) * sub->capacity);
	}
     }
   
  }
  return sub;
}


//finds which of two apints has a greater value
int apint_compare(const ApInt *left, const ApInt *right) {

 
  //checks if the capacity of the two ApInts differs
  if (left->capacity < right->capacity){
    return -1;
  }else if (right->capacity < left->capacity){
    return 1;
  }
  
  //if the capacity is the same, compares the values of each ApInt
  for (int i = (left->capacity - 1); i >= 0; i--){
    if (left->nums[i] < right->nums[i]){
      return -1;
    }else if (right->nums[i] < left->nums[i]){
      return 1;
    }
  }
  return 0;
}
