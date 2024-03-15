// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsId.h"
#include "UObject/Object.h"
#include "FlecsWorld.generated.h"

const FString GWorld_Name_Component = "WorldName";

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

	template <typename T>
	FORCEINLINE void SetSingleton(const T& Value) const
	{
		World->set<T>(Value);
	}

	template <typename First, typename Second>
	FORCEINLINE void SetSingleton(const First& InFirst, const Second& InSecond) const
	{
		World->set<First, Second>(InFirst, InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE void SetSingleton(const First& InFirst, const TArgs&... Args) const
	{
		World->set<First, TArgs...>(InFirst, Args...);
	}

	template <typename T>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE void RemoveSingleton(const Second& InSecond) const
	{
		World->remove<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE void RemoveSingleton(const FFlecsId& InSecond) const
	{
		World->remove<First>(InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE void RemoveSingleton() const
	{
		World->remove<First, TArgs...>();
	}
	
	template <typename T>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<First, Second>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD bool HasSingleton(const Second& InSecond) const
	{
		return World->has<First>(InSecond);
	}
	
	template <typename First>
	FORCEINLINE NO_DISCARD bool HasSingleton(const FFlecsId& InSecond) const
	{
		return World->has<First>(InSecond);
	}

	template <typename First, typename ...TArgs>
	FORCEINLINE NO_DISCARD bool HasSingleton() const
	{
		return World->has<First, TArgs...>();
	}

	template <typename T>
	FORCEINLINE NO_DISCARD T GetSingleton() const
	{
		return *World->get<T>();
	}

	template <typename First, typename Second>
	FORCEINLINE NO_DISCARD First GetSingleton(const Second& InSecond) const
	{
		return *World->get<First>(InSecond);
	}

	FORCEINLINE void Merge() const
	{
		World->merge();
	}

	FORCEINLINE void SetAutoMerge(const bool bInAutoMerge) const
	{
		World->set_automerge(bInAutoMerge);
	}

	FORCEINLINE NO_DISCARD FName GetName() const
	{
		return GetSingleton<FName>(GWorld_Name_Component);
	}

	template <typename T>
	FORCEINLINE void ImportModule() const
	{
		World->import<T>();
	}

private:
	flecs::world* World = nullptr;
}; // struct FFlecsWorld
