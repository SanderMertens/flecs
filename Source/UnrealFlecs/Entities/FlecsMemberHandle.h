// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "FlecsMemberHandle.generated.h"

// Avoid using this directly
USTRUCT()
struct UNREALFLECS_API FFlecsMemberHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsMemberHandle() = default;
	
	FORCEINLINE FFlecsMemberHandle(const flecs::member_t* InMember)
		: Member(InMember)
	{
	}

	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return Member != nullptr;
	}

	FORCEINLINE operator bool() const
	{
		return IsValid();
	}

	FORCEINLINE void SetMember(const flecs::member_t* InMember)
	{
		Member = InMember;
	}

	NO_DISCARD FORCEINLINE const flecs::member_t* GetFlecsMember() const
	{
		return Member;
	}

	FORCEINLINE operator const flecs::member_t*() const
	{
		return Member;
	}

	FORCEINLINE bool operator==(const FFlecsMemberHandle& Other) const
	{
		return Member == Other.Member;
	}

	FORCEINLINE bool operator!=(const FFlecsMemberHandle& Other) const
	{
		return !(*this == Other);
	}
	
	const flecs::member_t* Member = nullptr;
	
}; // struct FFlecsMemberHandle