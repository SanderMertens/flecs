// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsActorTag.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsActorTag
{
	GENERATED_BODY()
}; // struct FFlecsActorTag

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsActorTag, flecs::PairIsTag);
