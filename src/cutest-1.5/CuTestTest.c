#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CuTest.h"

void TestCuStrCopy(CuTest* tc)
{
	const char* old = "hello world";
	const char* newStr = CuStrCopy(old);
	CuAssert(tc, "old is new", strcmp(old, newStr) == 0);
}

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestCuStrCopy);

	return suite;
}
