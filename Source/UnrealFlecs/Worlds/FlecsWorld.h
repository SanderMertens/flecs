// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "SolidMacros/Macros.h"
#include "UObject/Object.h"
#include "FlecsWorld.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorld
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsWorld() = default;
	FORCEINLINE FFlecsWorld(flecs::world& InWorld) : World(&InWorld) {}

	FORCEINLINE void SetWorld(flecs::world& InWorld) { World = &InWorld; }

	FORCEINLINE NO_DISCARD flecs::world& GetWorld() { return *World; }
	FORCEINLINE NO_DISCARD const flecs::world& GetWorld() const { return *World; }

	FORCEINLINE NO_DISCARD operator flecs::world&() { return GetWorld(); }
	FORCEINLINE NO_DISCARD operator const flecs::world&() const { return GetWorld(); }

	FORCEINLINE NO_DISCARD bool IsValid() const { return World != nullptr; }

	FORCEINLINE NO_DISCARD bool operator!() const { return !IsValid(); }

	FORCEINLINE NO_DISCARD flecs::world* operator->() { return World; }
	FORCEINLINE NO_DISCARD const flecs::world* operator->() const { return World; }

	FORCEINLINE NO_DISCARD flecs::world& operator*() { return GetWorld(); }
	FORCEINLINE NO_DISCARD const flecs::world& operator*() const { return GetWorld(); }

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsWorld& Other) const
	{
		return World == Other.World;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsWorld& Other) const
	{
		return World != Other.World;
	}

	FORCEINLINE NO_DISCARD bool operator==(const flecs::world& Other) const
	{
		return World == &Other;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const flecs::world& Other) const
	{
		return World != &Other;
	}

private:
	flecs::world* World = nullptr;
}; // struct FFlecsWorld
