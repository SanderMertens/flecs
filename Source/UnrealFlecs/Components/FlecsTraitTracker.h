// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTraitTracker.generated.h"

USTRUCT()
struct FFlecsTraitTracker
{
	GENERATED_BODY()
public:
	robin_hood::unordered_map<flecs::entity_t, flecs::entity_t> ComponentProperties;
}; // struct FFlecsTraitTracker

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsTraitTracker, flecs::Sparse);


