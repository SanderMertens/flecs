// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include <functional>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsDependenciesComponent.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct alignas(64) UNREALFLECS_API FFlecsDependenciesComponent
{
	GENERATED_BODY()

public:
	robin_hood::unordered_flat_map<UClass*, std::function<void(TSolidNotNull<UObject*>,
	                                                           TSolidNotNull<UFlecsWorld*>,
	                                                           FFlecsEntityHandle)>> Dependencies;
}; // struct FFlecsDependenciesComponent

REGISTER_FLECS_COMPONENT(FFlecsDependenciesComponent,
	[](flecs::world InWorld, flecs::untyped_component InComponent)
	{
		InComponent.add(flecs::Sparse);
	});
