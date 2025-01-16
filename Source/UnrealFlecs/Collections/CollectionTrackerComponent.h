// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "Properties/FlecsComponentProperties.h"
#include "CollectionTrackerComponent.generated.h"

USTRUCT()
struct UNREALFLECS_API FCollectionTrackerComponent
{
    GENERATED_BODY()

public:
    robin_hood::unordered_map<FString, FFlecsEntityHandle> ComponentProperties;
    
}; // struct FCollectionTrackerComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FCollectionTrackerComponent, flecs::Sparse);
