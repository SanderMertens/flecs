// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Worlds/FlecsWorld.h"
#include "FlecsWorldPtrComponent.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorldPtrComponent
{
	GENERATED_BODY()

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsWorldPtrComponent& InComponent)
	{
		return GetTypeHash(InComponent.World.Get());
	}

	FORCEINLINE FFlecsWorldPtrComponent() = default;

	FORCEINLINE FFlecsWorldPtrComponent(UFlecsWorld* InWorld)
		: World(InWorld)
	{
	}

	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const { return World.Get(); }

	FORCEINLINE NO_DISCARD bool IsValid() const
	{
		return World.IsValid();
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(UFlecsWorld* InWorld)
	{
		World = InWorld;
		return *this;
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(const FFlecsWorldPtrComponent& InComponent)
	{
		World = InComponent.World;
		return *this;
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(const FFlecsWorldPtrComponent&& InComponent)
	{
		World = InComponent.World;
		return *this;
	}

	FORCEINLINE bool operator==(const UFlecsWorld* InWorld) const
	{
		return World == InWorld;
	}

	FORCEINLINE bool operator!=(const UFlecsWorld* InWorld) const
	{
		return World != InWorld;
	}

	FORCEINLINE bool operator==(const FFlecsWorldPtrComponent& InComponent) const
	{
		return World == InComponent.World;
	}

	FORCEINLINE bool operator!=(const FFlecsWorldPtrComponent& InComponent) const
	{
		return World != InComponent.World;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Flecs")
	TWeakObjectPtr<UFlecsWorld> World;
	
}; // struct FFlecsWorldPtrComponent

FORCEINLINE NO_DISCARD UFlecsWorld* ToFlecsWorld(const flecs::world& InWorld)
{
	return InWorld.get_mut<FFlecsWorldPtrComponent>()->GetFlecsWorld();
}
