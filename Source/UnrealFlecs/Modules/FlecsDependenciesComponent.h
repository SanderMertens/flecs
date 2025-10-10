// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include <functional>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsDependenciesComponent.generated.h"

class UFlecsWorld;

struct FFlecsDependencyFunctionDefinition
{
	using FDependencyFunctionType = std::function<void(UObject*, const UFlecsWorld*, FFlecsEntityHandle)>;
	
	FDependencyFunctionType Function;

	void Call(const TSolidNotNull<UObject*> InDependencyObject,
	                    const TSolidNotNull<const UFlecsWorld*> InWorld,
	                    const FFlecsEntityHandle& InDependencyEntity) const
	{
		solid_check(Function);

		std::invoke(Function, InDependencyObject, InWorld, InDependencyEntity);
	}
	
}; // struct FFlecsDependencyFunctionDefiniton

USTRUCT()
struct UNREALFLECS_API FFlecsDependenciesComponent
{
	GENERATED_BODY()

public:
	TSortedMap<UClass*, FFlecsDependencyFunctionDefinition> DependencyFunctionPtrs;
	
	/*robin_hood::unordered_map<UClass*, std::function<void(TSolidNotNull<UObject*>,
	                                                           TSolidNotNull<UFlecsWorld*>,
	                                                           FFlecsEntityHandle)>> Dependencies;*/
}; // struct FFlecsDependenciesComponent

REGISTER_FLECS_COMPONENT(FFlecsDependenciesComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Sparse);
	});
