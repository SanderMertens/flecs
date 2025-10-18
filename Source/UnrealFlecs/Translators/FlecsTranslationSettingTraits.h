// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTranslationSettingTraits.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTranslationPropertyTrait
{
	GENERATED_BODY()
}; // struct FFlecsTranslationPropertyTrait

REGISTER_FLECS_COMPONENT(FFlecsTranslationPropertyTrait,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::PairIsTag)
			.Add(flecs::Relationship);
	})

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsToUnreal
{
	GENERATED_BODY()
}; // struct FFlecsToUnreal

REGISTER_FLECS_COMPONENT(FFlecsToUnreal,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	})

USTRUCT(BlueprintType)
struct UNREALFLECS_API FUnrealToFlecs
{
	GENERATED_BODY()
}; // struct FUnrealToFlecs

REGISTER_FLECS_COMPONENT(FUnrealToFlecs,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	})

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsToBoth
{
	GENERATED_BODY()
}; // struct FFlecsToBoth

REGISTER_FLECS_COMPONENT(FFlecsToBoth,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.Add(flecs::Target);
	})