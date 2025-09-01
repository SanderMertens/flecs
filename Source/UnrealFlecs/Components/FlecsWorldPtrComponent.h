// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Worlds/FlecsWorld.h"

#include "FlecsWorldPtrComponent.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorldPtrComponent
{
	GENERATED_BODY()

	NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsWorldPtrComponent& InComponent)
	{
		return GetTypeHash(InComponent.World);
	}

public:
	FORCEINLINE FFlecsWorldPtrComponent() = default;

	FORCEINLINE FFlecsWorldPtrComponent(UFlecsWorld* InWorld)
		: World(InWorld)
	{
	}

	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return World;
	}

	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return ::IsValid(World);
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

	FORCEINLINE FFlecsWorldPtrComponent& operator=(const FFlecsWorldPtrComponent&& InComponent) NOEXCEPT
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
	TObjectPtr<UFlecsWorld> World;
	
}; // struct FFlecsWorldPtrComponent
