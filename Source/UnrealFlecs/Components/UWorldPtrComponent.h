// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Properties/FlecsComponentProperties.h"
#include "UWorldPtrComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FUWorldPtrComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FUWorldPtrComponent() = default;
	FORCEINLINE FUWorldPtrComponent(UWorld* InWorld) : World(InWorld) {}
	
	FORCEINLINE void SetWorld(UWorld* InWorld) { World = InWorld; }
	FORCEINLINE NO_DISCARD UWorld* GetWorld() const { return World.Get(); }
	FORCEINLINE NO_DISCARD bool IsValid() const { return World.IsValid(false, true); }

	FORCEINLINE FUWorldPtrComponent& operator=(UWorld* InWorld)
	{
		World = InWorld;
		return *this;
	}

	FORCEINLINE FUWorldPtrComponent& operator=(const FUWorldPtrComponent& InComponent)
	{
		World = InComponent.World;
		return *this;
	}

	FORCEINLINE bool operator==(const FUWorldPtrComponent& InComponent) const
	{
		return World == InComponent.World;
	}

	FORCEINLINE bool operator!=(const FUWorldPtrComponent& InComponent) const
	{
		return !(*this == InComponent);
	}

	FORCEINLINE bool operator==(const UWorld* InWorld) const
	{
		return World == InWorld;
	}

	FORCEINLINE bool operator!=(const UWorld* InWorld) const
	{
		return !(*this == InWorld);
	}

	FORCEINLINE bool operator==(const TWeakObjectPtr<UWorld>& InWorld) const
	{
		return World == InWorld;
	}

	FORCEINLINE bool operator!=(const TWeakObjectPtr<UWorld>& InWorld) const
	{
		return !(*this == InWorld);
	}

	UPROPERTY(BlueprintReadOnly, Category = "Flecs")
	TWeakObjectPtr<UWorld> World;
	
}; // struct FUWorldPtrComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FUWorldPtrComponent, flecs::Sparse)
