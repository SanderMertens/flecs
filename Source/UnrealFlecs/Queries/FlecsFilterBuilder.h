// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsFilterBuilder.generated.h"

USTRUCT(BlueprintType)
struct FFlecsFilterBuilder
{
    GENERATED_BODY()
    
    flecs::filter_builder<> FilterBuilder;
}; // struct FFlecsFilterBuilder
