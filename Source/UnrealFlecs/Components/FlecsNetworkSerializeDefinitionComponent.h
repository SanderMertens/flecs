// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Entities/FlecsComponentHandle.h"
#include "Properties/FlecsComponentProperties.h"

#include "FlecsNetworkSerializeDefinitionComponent.generated.h"

// Sparse
USTRUCT()
struct UNREALFLECS_API FFlecsNetworkSerializeDefinitionComponent
{
	GENERATED_BODY()

public:
	FFlecsNetworkSerializeDefinitionComponent();

	Unreal::Flecs::FEntityNetSerializeFunction NetSerializeFunction = nullptr;
	
}; // struct FFlecsNetworkSerializeDefinitionComponent

REGISTER_FLECS_COMPONENT(FFlecsNetworkSerializeDefinitionComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		
	});
