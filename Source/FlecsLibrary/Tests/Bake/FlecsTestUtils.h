// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"

#define test_assert(cond) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
	{ \
		if (!CurrentTest->TestTrue(TEXT(#cond), (cond))) \
		{ \
			return; \
		} \
	} 

#define test_bool(v1, v2) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestEqual(TEXT(#v1 " == " #v2), (v1), (v2))

#define test_true(v) test_bool((v), true)
#define test_false(v) test_bool((v), false)

#define test_int(v1, v2) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestEqual(TEXT(#v1 " == " #v2), static_cast<int64>(v1), static_cast<int64>(v2))

#define test_uint(v1, v2) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestEqual(TEXT(#v1 " == " #v2), static_cast<uint64>(v1), static_cast<uint64>(v2))

#define test_str(v1, v2) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestEqual(TEXT(#v1 " == " #v2), FString(v1), FString(v2))

#define test_null(v) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestNull(TEXT(#v " is null"), (v))

#define test_not_null(v) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestNotNull(TEXT(#v " is not null"), (v))

#define test_ptr(v1, v2) \
	if (FAutomationTestBase* CurrentTest = FAutomationTestFramework::Get().GetCurrentTest()) \
		CurrentTest->TestEqual(TEXT(#v1 " == " #v2), static_cast<void*>(v1), static_cast<void*>(v2))

#endif // WITH_AUTOMATION_TESTS
