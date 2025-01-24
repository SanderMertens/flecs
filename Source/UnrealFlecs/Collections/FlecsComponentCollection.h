// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "FlecsComponentCollection.generated.h"

USTRUCT(BlueprintType)
struct FFlecsComponentCollection
{
	GENERATED_BODY()
}; // struct FFlecsComponentCollection

REGISTER_USTRUCT_FLECS_COMPONENT(FFlecsComponentCollection, [](flecs::world InWorld, flecs::untyped_component InComponent)
{
	InComponent.add(flecs::With, flecs::Prefab);
	InComponent.add(flecs::OnInstantiate, flecs::DontInherit);
});