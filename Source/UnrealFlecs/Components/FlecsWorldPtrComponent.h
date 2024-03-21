// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	FORCEINLINE FFlecsWorldPtrComponent(UFlecsWorld* InWorld, UWorld* InOwningWorld)
		: World(InWorld)
		, OwningWorld(InOwningWorld)
	{
	}

	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const { return World.Get(); }
	FORCEINLINE NO_DISCARD UWorld* GetOwningWorld() const { return OwningWorld.Get(); }

	FORCEINLINE NO_DISCARD bool IsValid() const
	{
		return GetFlecsWorld() && GetOwningWorld();
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(UFlecsWorld* InWorld)
	{
		World = InWorld;
		return *this;
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(UWorld* InOwningWorld)
	{
		OwningWorld = InOwningWorld;
		return *this;
	}

	FORCEINLINE FFlecsWorldPtrComponent& operator=(const FFlecsWorldPtrComponent& InComponent)
	{
		World = InComponent.World;
		OwningWorld = InComponent.OwningWorld;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsWorldPtrComponent& InComponent) const
	{
		return World == InComponent.World && OwningWorld == InComponent.OwningWorld;
	}

	FORCEINLINE bool operator!=(const FFlecsWorldPtrComponent& InComponent) const
	{
		return World != InComponent.World || OwningWorld != InComponent.OwningWorld;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Flecs")
	TWeakObjectPtr<UFlecsWorld> World;

	UPROPERTY(BlueprintReadOnly, Category = "Flecs")
	TWeakObjectPtr<UWorld> OwningWorld;
	
}; // struct FFlecsWorldPtrComponent
