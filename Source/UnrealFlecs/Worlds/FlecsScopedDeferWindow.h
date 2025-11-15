// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/WeakObjectPtr.h"

#include "Types/SolidNotNull.h"

class UFlecsWorld;

// @TODO: this isn't RAII compliant with multiple worlds.
// @TODO: add tests for this
// RAII helper to create a Flecs Defer Window on a given Flecs World.
struct UNREALFLECS_API FFlecsScopedDeferWindow
{
public:
	explicit FFlecsScopedDeferWindow(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld);
	~FFlecsScopedDeferWindow();

	FFlecsScopedDeferWindow(const FFlecsScopedDeferWindow&) = delete;
	FFlecsScopedDeferWindow& operator=(const FFlecsScopedDeferWindow&) = delete;

	FFlecsScopedDeferWindow() = delete;

	FORCEINLINE FFlecsScopedDeferWindow(FFlecsScopedDeferWindow&& Other) noexcept
		: FlecsWorld(Other.FlecsWorld)
	{
		Other.FlecsWorld.Reset();
	}

	FORCEINLINE FFlecsScopedDeferWindow& operator=(FFlecsScopedDeferWindow&& Other) noexcept
	{
		if LIKELY_IF(this != &Other)
		{
			FlecsWorld = Other.FlecsWorld;
			Other.FlecsWorld.Reset();
		}

		return *this;
	}

	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return FlecsWorld.IsValid();
	}

	NO_DISCARD FORCEINLINE const UFlecsWorld* GetFlecsWorld() const
	{
		return FlecsWorld.Get();
	}

	NO_DISCARD FORCEINLINE TSolidNotNull<const UFlecsWorld*> GetFlecsWorldChecked() const
	{
		solid_checkf(FlecsWorld.IsValid(), TEXT("FlecsWorld is not valid!"));
		return FlecsWorld.Get();
	}
	
	TWeakObjectPtr<const UFlecsWorld> FlecsWorld;
	
}; // struct FFlecsScopedDeferWindow


