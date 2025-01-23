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

REGISTER_COMPONENT_TRAIT_TAG(FFlecsComponentCollection,
	FFlecsId::MakePair(flecs::With, flecs::Prefab), FFlecsId::MakePair(flecs::OnInstantiate, flecs::DontInherit));
	