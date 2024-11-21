///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

// We can't override EA_ASSERTS failure callback so we won't be able to test EAAssert in C but at least we can
// prove it builds our macros correctly

#include <stdio.h>

#include <EAAssert/eaassert.h>

#ifdef _MSC_VER
	#pragma warning(disable: 4127)
#endif

int EAMain(int argc, char** argv)
{
	const char* testMsg = "The quick brown fox jumps over the lazy dog.";
	int nErrorCount = 0;
	int expectToBeSet = 0;

	EA_COMPILETIME_ASSERT(1);
	EA_ASSERT(1);
	
	EA_UNUSED(testMsg);
	EA_UNUSED(expectToBeSet);

	EA_ASSERT_M(4 == 4, testMsg);
	EA_ASSERT_MESSAGE(4 == 4, testMsg);
	EA_ASSERT_MSG(4 == 4, testMsg);
	EA_ASSERT_FORMATTED(8 == 8, ("The quick brown fox jumps over the %s dog.", "lazy"));
	EA_ASSERT(3 % 8 == 3);

	
	expectToBeSet = 0;
	if (0)
		EA_ASSERT(0);
	else
		expectToBeSet = 1;
	EA_ASSERT(expectToBeSet == 1);

	EA_PANIC_M(4 == 4, testMsg);
	EA_PANIC_MESSAGE(4 == 4, testMsg);
	EA_PANIC_MSG(4 == 4, testMsg);
	EA_PANIC_FORMATTED(8 == 8, ("The quick brown fox jumps over the %s dog.", "lazy"));
	EA_PANIC(3 % 8 == 3);
 
	return nErrorCount;
}
