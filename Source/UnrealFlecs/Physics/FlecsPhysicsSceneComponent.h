// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsPhysicsSceneComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPhysicsSceneComponent
{
	GENERATED_BODY()

public:
	FPhysScene* PhysicsScene = nullptr;

	FORCEINLINE operator FPhysScene*() const { return PhysicsScene; }
	FORCEINLINE FPhysScene* operator->() const { return PhysicsScene; }
	FORCEINLINE FPhysScene& operator*() const { return *PhysicsScene; }
	
	FORCEINLINE bool operator==(const FFlecsPhysicsSceneComponent& InComponent) const
	{
		return PhysicsScene == InComponent.PhysicsScene;
	}

	FORCEINLINE bool operator!=(const FFlecsPhysicsSceneComponent& InComponent) const
	{
		return !(*this == InComponent);
	}
	
}; // struct FFlecsPhysicsSceneComponent

REGISTER_FLECS_COMPONENT(FFlecsPhysicsSceneComponent,
	[](flecs::world World, const FFlecsComponentHandle& Component)
	{
		Component.Add(flecs::Sparse);
	});