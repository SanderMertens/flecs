// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsNetworkTrait.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsNetworkTrait
{
    GENERATED_BODY()
}; // struct FFlecsNetworkTrait

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsNetworkTrait, flecs::Trait)
