///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
	#pragma warning(disable: 4127)
#endif

#include "EAMain/EAEntryPointMain.inl"
#include "EATest/EASTLNewOperatorGuard.inl"


namespace
{
	const char* expectedExpr = 0;
	const char* expectedFilename = 0;
	const char* expectedFunction = 0;
	const char* expectedMessage = 0;
	int         expectedLine = 0;
	bool        assertCalled = false;
	bool        expressionCalled = false;
}

static void Expect(const char* expr, const char* filename, int line, const char* function, const char* msg)
{
	expectedExpr = expr;
	expectedFilename = filename;
	expectedFunction = function;
	expectedMessage = msg;
	expectedLine = line;
}

// similar to assert in libc, but this function is not compiled out in a
// release build
// \param condition should be true when EA_ASSERT_ENABLED is defined,
// false when EA_ASSERT_ENABLED is not defined.
// \param always the condition should be true regardless of whether
// EA_ASSERT_ENABLED is defined or not.
static void TestCondition(bool condition, bool always)
{
	if (always)
	{
		if (!condition)
		{
			EA::EAMain::Report("Test failed\n");
			EA_DEBUG_BREAK();
		}

		return;
	}

#if defined(EA_ASSERT_ENABLED)
	if (!condition)
	{
		EA::EAMain::Report("Test failed\n");
		EA_DEBUG_BREAK();
	}
#else
	if (condition)
	{
		EA::EAMain::Report("Test failed\n");
		EA_DEBUG_BREAK();
	}
#endif
}

static bool EAAssertFailure(const char* expr, const char* filename, int line, const char* function, const char* msg, va_list args)
{
	assertCalled = true;

	TestCondition(0 == strcmp(expr, expectedExpr), false);
	TestCondition(0 != strstr(filename, expectedFilename), false);
	TestCondition(0 != strstr(function, expectedFunction), false);

	const int size = 2048;
	char fmtMsg[size + 1] = {};
	vsprintf(fmtMsg, msg, args);

	TestCondition(0 == strcmp(fmtMsg, expectedMessage), false);
	TestCondition(line == expectedLine, false);

	return false;
}

bool TestExpression(bool *x)
{
	*x = !*x;

	return *x;
}

EA_COMPILETIME_ASSERT(true);

int EAMain(int argc, char**)
{    
	EA::Assert::SetFailureCallback(&EAAssertFailure);

	EA_COMPILETIME_ASSERT(true);

	#ifndef EA_COMPILER_GNUC
	// TODO(rparolin): Disabling this test on GCC because the compiler correctly wanrs that the output of EA_OFFSETOF is
	// not constexpr.  Unfortunately, the implementation is attempting to support calculating offsets within
	// non-standard-layout types which is not supported by the Cpp Standard.  Fix required.
	{
		struct OffsetOfTestStruct
		{
			int member;
		};

		// Note:
		//
		// User code currently exists that uses EA_COMPILETIME_ASSERT with results from EA_OFFSETOF.
		// Modifying the implementation of either EA_COMPILETIME_ASSERT or EA_OFFSETOF could cause this
		// code to break.  (We have observed failures on clang.)
		//
		// See also:
		//   - EAStdC's EAOffsetOf works properly with static_assert, but also needs to suppress compiler
		//     warnings to compile warning free in all situations.  We chose not to perform this suppression
		//     in EABase's EA_OFFSETOF because it would leak into files including eabase.
		EA_COMPILETIME_ASSERT(EA_OFFSETOF(OffsetOfTestStruct, member) == 0);
	}
	#endif

	assertCalled = false;
	EA_ASSERT(true);
	TestCondition(assertCalled == false, true);

	const char* testMsg = "The quick brown fox jumps over the lazy dog.";

	const char* mainName = "EAMain";

	assertCalled = false;
	Expect("3 == 4", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_ASSERT_M(3 == 4, testMsg);
	TestCondition(assertCalled, false);

	assertCalled = false;
	Expect("3 == 4", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_ASSERT_MESSAGE(3 == 4, testMsg);
	TestCondition(assertCalled, false);

	assertCalled = false;
	Expect("3 == 4", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_ASSERT_MSG(3 == 4, testMsg);
	TestCondition(assertCalled, false);

	assertCalled = false;
	Expect("8 == 2", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_ASSERT_FORMATTED(8 == 2, ("The quick brown fox jumps over the %s dog.", "lazy"));
	TestCondition(assertCalled, false);

	assertCalled = false;
	Expect("3 % 8 == 2", __FILE__, __LINE__ + 1, mainName, "");
	EA_ASSERT(3 % 8 == 2);
	TestCondition(assertCalled, false);

	bool expectToBeSet = false;
	if (false)
		EA_ASSERT(false);
	else
		expectToBeSet = true;
	TestCondition(expectToBeSet, true);

	// Regression test for checking whether an expression is compiled
	// out when asserts are disabled.
	EA_ASSERT(TestExpression(&expressionCalled));
	TestCondition(expressionCalled, false);

	// Ensure that the unconditional assertion format behaves correctly
	Expect("EA_FAIL", __FILE__, __LINE__ + 1, mainName, "");
	EA_FAIL();

	Expect("EA_FAIL", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_FAIL_MSG(testMsg);

	Expect("EA_FAIL", __FILE__, __LINE__ + 1, mainName, testMsg);
	EA_FAIL_FORMATTED(("%s", testMsg));

	EA::EAMain::Report("Test complete - all tests passed.\n");

	return 0;
}
