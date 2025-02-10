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

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsWorldPtrComponent& InComponent)
	{
		return GetTypeHash(InComponent.World);
	}

	FORCEINLINE FFlecsWorldPtrComponent() = default;

	FORCEINLINE FFlecsWorldPtrComponent(UFlecsWorld* InWorld)
		: World(InWorld)
	{
	}

	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const
	{
		return World;
	}

	FORCEINLINE NO_DISCARD bool IsValid() const
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

FORCEINLINE NO_DISCARD UFlecsWorld* ToFlecsWorld(const flecs::world& InWorld)
{
	static TWeakObjectPtr<UFlecsWorld> CachedWorld;

	if (CachedWorld.IsValid() && CachedWorld.Get()->World == InWorld)
	{
		return CachedWorld.Get();
	}
	
	CachedWorld = InWorld.get_mut<FFlecsWorldPtrComponent>()->GetFlecsWorld();
	solid_check(CachedWorld.IsValid());
	return CachedWorld.Get();
}
