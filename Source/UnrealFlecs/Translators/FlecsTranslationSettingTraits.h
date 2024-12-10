// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTranslationSettingTraits.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsToUnreal
{
	GENERATED_BODY()
}; // struct FFlecsToUnreal

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsToUnreal, flecs::PairIsTag)

USTRUCT(BlueprintType)
struct UNREALFLECS_API FUnrealToFlecs
{
	GENERATED_BODY()
}; // struct FUnrealToFlecs

REGISTER_COMPONENT_TAG_PROPERTIES(FUnrealToFlecs, flecs::PairIsTag)

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsToBoth
{
	GENERATED_BODY()
}; // struct FFlecsToBoth

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsToBoth, flecs::PairIsTag)