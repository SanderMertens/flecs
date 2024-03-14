// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsId.h"
#include "UObject/Object.h"
#include "FlecsWorld.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorld
{
	GENERATED_BODY()

public:
	FFlecsWorld() = default;
	FORCEINLINE FFlecsWorld(flecs::world& InWorld) : World(&InWorld) {}
	FORCEINLINE FFlecsWorld(flecs::world* InWorld) : World(InWorld) {}

	FORCEINLINE void SetWorld(flecs::world& InWorld) { World = &InWorld; }

	flecs::world& GetFlecsWorld() { return *World; }
	const flecs::world& GetFlecsWorld() const { return *World; }

	operator flecs::world&() { return GetFlecsWorld(); }
	operator const flecs::world&() const { return GetFlecsWorld(); }

	FORCEINLINE NO_DISCARD bool IsValid() const { return World != nullptr; }

	FORCEINLINE bool operator!() const { return !IsValid(); }

	FORCEINLINE flecs::world* operator->() { return World; }
	FORCEINLINE const flecs::world* operator->() const { return World; }

	FORCEINLINE flecs::world& operator*() { return GetFlecsWorld(); }
	FORCEINLINE const flecs::world& operator*() const { return GetFlecsWorld(); }

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

	template <typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function)
	{
		World->each(std::forward<FunctionType>(Function));
	}

	template <typename T, typename FunctionType>
	FORCEINLINE void ForEach(FunctionType&& Function)
	{
		World->each<T>(std::forward<FunctionType>(Function));
	}

	template <typename FunctionType>
	FORCEINLINE void ForEach(const FFlecsId& InTermId, FunctionType&& Function)
	{
		World->each(InTermId.GetFlecsId(), std::forward<FunctionType>(Function));
	}

	FORCEINLINE void Merge() const
	{
		World->merge();
	}

private:
	flecs::world* World = nullptr;
}; // struct FFlecsWorld
