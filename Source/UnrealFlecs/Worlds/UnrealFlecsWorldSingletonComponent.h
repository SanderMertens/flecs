// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "UnrealFlecsWorldSingletonComponent.generated.h"

USTRUCT(BlueprintType, MinimalAPI)
struct FUnrealFlecsWorldSingletonComponent
{
	GENERATED_BODY()
}; // struct FUnrealFlecsWorldSingletonComponent

REGISTER_FLECS_COMPONENT(FUnrealFlecsWorldSingletonComponent,
	[](flecs::world World, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.Add(flecs::Singleton);
	});