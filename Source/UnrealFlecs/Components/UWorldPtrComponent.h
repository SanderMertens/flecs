// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "Engine/World.h"

#include "SolidMacros/Macros.h"

#include "UWorldPtrComponent.generated.h"

class UWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FUWorldPtrComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FUWorldPtrComponent() = default;
	
	FORCEINLINE FUWorldPtrComponent(UWorld* InWorld)
		: World(InWorld)
	{
	}
	
	FORCEINLINE void SetWorld(UWorld* InWorld)
	{
		World = InWorld;
	}
	
	NO_DISCARD FORCEINLINE UWorld* GetWorld() const
	{
		return World;
	}
	
	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return ::IsValid(World);
	}

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
	TObjectPtr<UWorld> World;
	
}; // struct FUWorldPtrComponent

template<>
struct TStructOpsTypeTraits<FUWorldPtrComponent> : public TStructOpsTypeTraitsBase2<FUWorldPtrComponent>
{
	enum
	{
		
	};
}; // struct TStructOpsTypeTraits<FUWorldPtrComponent>
