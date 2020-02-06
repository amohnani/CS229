/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
	ApInt *ap0;
	ApInt *ap1;
	ApInt *ap110660361;
	ApInt *max1;
	/* TODO: add additional fields of test fixture */
  ApInt *hex1;
  ApInt *hex2;
  ApInt *hex3;
  ApInt *hex4;
  ApInt *hex5;
} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testLshiftN(TestObjs *objs);
void testCompare(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
/* TODO: add more test function prototypes */
void testCreateFromHex(TestObjs *objs);

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromHex);
	TEST(testCreateFromU64);
	TEST(testHighestBitSet);
	TEST(testLshiftN);
	TEST(testCompare);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testSub);
	/* TODO: use TEST macro to execute more test functions */

	TEST_FINI();
}

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->ap0 = apint_create_from_u64(0UL);
	objs->ap1 = apint_create_from_u64(1UL);
	objs->ap110660361 = apint_create_from_u64(110660361UL);
	objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);
	/* TODO: initialize additional members of test fixture */
  objs->hex1 = apint_create_from_hex("0");
  objs->hex2 = apint_create_from_hex("AAAAAAAa");
  objs->hex3 = apint_create_from_hex("111111111111111111111111111111111");
  objs->hex4 = apint_create_from_hex("55555555");
  objs->hex5 = apint_create_from_hex("56ea5672654677e0a000009b635246cdD987654e321aAB54");
	return objs;
}

void cleanup(TestObjs *objs) {
	apint_destroy(objs->ap0);
	apint_destroy(objs->ap1);
	apint_destroy(objs->ap110660361);
	apint_destroy(objs->max1);
	/* TODO: destroy additional members of test fixture */
	apint_destroy(objs->hex1);
	apint_destroy(objs->hex2);
	apint_destroy(objs->hex3);
	apint_destroy(objs->hex4);
	apint_destroy(objs->hex5);
	free(objs);
}

void testCreateFromHex(TestObjs *objs){
  ASSERT(0UL == apint_get_bits(objs->hex1, 0));
  ASSERT(2863311530UL == apint_get_bits(objs->hex2,0));
  ASSERT(1229782938247303441UL == apint_get_bits(objs->hex3,0));
  ASSERT(1229782938247303441UL == apint_get_bits(objs->hex3, 1));
  ASSERT(1UL == apint_get_bits(objs->hex3,2));

  
}

void testCreateFromU64(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}

void testHighestBitSet(TestObjs *objs) {
	ASSERT(-1 == apint_highest_bit_set(objs->ap0));
	ASSERT(0 == apint_highest_bit_set(objs->ap1));
	ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
	ASSERT(63 == apint_highest_bit_set(objs->max1));
	ASSERT(128 == apint_highest_bit_set(objs->hex3));
}

void testLshiftN(TestObjs *objs) {
	ApInt *result;
	
	result = apint_lshift(objs->ap1);
	ASSERT(2UL == apint_get_bits(result,0));
	apint_destroy(result);
	
	result = apint_lshift(objs->hex2);
	ASSERT(5726623060UL == apint_get_bits(result, 0));
	apint_destroy(result);
	
	result = apint_lshift(objs->max1);
	ASSERT(1UL == apint_get_bits(result, 1));
	apint_destroy(result);
	
	result = apint_lshift_n(objs->ap0, 17);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap1, 17);
	ASSERT(0x20000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap110660361, 17);
	ASSERT(0xD3116120000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);
}

void testCompare(TestObjs *objs) {

  ApInt *result;
  /* 1 > 0 */
	ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
	/* 0 < 1 */
	ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
	/* 110660361 > 0 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
	/* 110660361 > 1 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
	/* 0 < 110660361 */
	ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
	/* 1 < 110660361 */
	ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);
	
	result = apint_lshift_n(objs->ap1, 32);
	ASSERT(apint_compare(result, objs->hex3) < 0);

	ASSERT(apint_compare(result, objs->hex2) > 0);
	apint_destroy(result);

	//checks equal apints
	result = apint_lshift_n(objs->hex4, 1);
       	ASSERT(apint_compare(result,objs->hex2) == 0);
	apint_destroy(result);
}

void testFormatAsHex(TestObjs *objs) {
	char *s;

		ASSERT(0 == strcmp("0", (s = apint_format_as_hex(objs->ap0))));
		free(s);

	
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
	free(s);

	ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
	free(s);

	ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
	free(s);

	ASSERT(0 == strcmp("aaaaaaaa", (s = apint_format_as_hex(objs->hex2))));
	free(s);
	ASSERT(0 == strcmp("111111111111111111111111111111111", (s = apint_format_as_hex(objs->hex3))));
	free(s);

	s = apint_format_as_hex(objs->hex5);
	ASSERT(0 == strcmp("56ea5672654677e0a000009b635246cdd987654e321aab54", s));
      	free(s);
}

void testAdd(TestObjs *objs) {
  ApInt *sum, *a, *b;
	char *s;

	/* 0 + 0 = 0 */
	sum = apint_add(objs->ap0, objs->ap0);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 0 = 1 */
	sum = apint_add(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 1 = 2 */
	sum = apint_add(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 110660361 + 1 = 110660362 */
	sum = apint_add(objs->ap110660361, objs->ap1);
	ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* FFFFFFFFFFFFFFFF + 1 = 10000000000000000 */
	sum = apint_add(objs->max1, objs->ap1);
	s = apint_format_as_hex(sum);
	ASSERT(0 == strcmp("10000000000000000", s));
	apint_destroy(sum);
	free(s);

	//checks carrying over through multiple blocks
	a = apint_create_from_hex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
	sum = apint_add(a, objs->ap1);
	s = apint_format_as_hex(sum);
	ASSERT(0 == strcmp("100000000000000000000000000000000", s));
	apint_destroy(sum);
	free(s);

	//again, checks carrying over multiple blocks
	b = apint_create_from_hex("2");
	sum = apint_add(a, b);
	s = apint_format_as_hex(sum);
	ASSERT(apint_get_bits(sum, 0) == 1);
	ASSERT(0 == strcmp("100000000000000000000000000000001", s));
	apint_destroy(a);
	apint_destroy(b);
	apint_destroy(sum);
	free(s);

	//checks addition over multiple blocks
	a = apint_create_from_hex("aaaaaaaaaaaaaaaaa1");
	sum = apint_add(a, objs->ap1);
	s = apint_format_as_hex(sum);
	ASSERT(0 == strcmp(s, "aaaaaaaaaaaaaaaaa2"));
	apint_destroy(a);
	apint_destroy(sum);
	free(s);
}

void testSub(TestObjs *objs) {
  ApInt *a, *b, *diff;
	char *s;

	/* subtracting 1 from ffffffffffffffff is fffffffffffffffe */
	diff = apint_sub(objs->max1, objs->ap1);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp("fffffffffffffffe", s));
	apint_destroy(diff);
	free(s);

	/* subtracting 0 from 1 is 1 */
	diff = apint_sub(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap1));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 1 is 0 */
	diff = apint_sub(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap0));
	apint_destroy(diff);
	free(s);

	//checks subtraction of two apints with different array lengths
	a = apint_lshift_n(objs->ap1, 64);
	diff = apint_sub(a, objs->max1);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp("1", s));
	  apint_destroy(diff);
	apint_destroy(a);
	free(s);

	/* subtracting 1 from 0 can't be represented because it is negative */
	ASSERT(NULL == apint_sub(objs->ap0, objs->ap1));

	/* test involving larger values */
	a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	diff = apint_sub(b, objs->ap1);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp(s,"9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301db"));
	free(s);
	apint_destroy(diff);       


	//checks large subtraction
	diff = apint_sub(a, b);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		s));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	//checks eliminating multiple blocks
	a = apint_create_from_hex("100000000000000000000000000000000");
	b = apint_create_from_hex("ffffffffffffffffffffffffffffffff");
	diff = apint_sub(a, b);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp("1", s));
	apint_destroy(diff);
	apint_destroy(b);
	free(s);

	//checks borrowing over multiple blocks
	diff = apint_sub(a,objs->ap1);
	s = apint_format_as_hex(diff);
	ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffff", s));
	apint_destroy(a);
	apint_destroy(diff);
	free(s);
}

