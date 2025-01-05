// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "flecs.h"
#include "Properties/FlecsComponentProperties.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsDependenciesComponent.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsDependenciesComponent
{
	GENERATED_BODY()

public:
	robin_hood::unordered_flat_map<UClass*, std::function<void(UObject*, UFlecsWorld*, FFlecsEntityHandle)>> Dependencies;
}; // struct FFlecsDependenciesComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsDependenciesComponent, flecs::Sparse);
