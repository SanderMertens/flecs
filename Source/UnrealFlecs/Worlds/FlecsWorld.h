// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "UObject/Object.h"
#include "FlecsWorld.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorld
{
	GENERATED_BODY()

public:
	FFlecsWorld() = default;
	FFlecsWorld(flecs::world& InWorld) : World(&InWorld) {}

	void SetWorld(flecs::world& InWorld) { World = &InWorld; }

	flecs::world& GetFlecsWorld() { return *World; }
	const flecs::world& GetFlecsWorld() const { return *World; }

	operator flecs::world&() { return GetFlecsWorld(); }
	operator const flecs::world&() const { return GetFlecsWorld(); }

	bool IsValid() const { return World != nullptr; }

	bool operator!() const { return !IsValid(); }

	flecs::world* operator->() { return World; }
	const flecs::world* operator->() const { return World; }

	flecs::world& operator*() { return GetFlecsWorld(); }
	const flecs::world& operator*() const { return GetFlecsWorld(); }

	bool operator==(const FFlecsWorld& Other) const
	{
		return World == Other.World;
	}

	bool operator!=(const FFlecsWorld& Other) const
	{
		return World != Other.World;
	}

	bool operator==(const flecs::world& Other) const
	{
		return World == &Other;
	}

	bool operator!=(const flecs::world& Other) const
	{
		return World != &Other;
	}

private:
	flecs::world* World = nullptr;
}; // struct FFlecsWorld
