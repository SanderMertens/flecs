// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "ConsoleVariableComponents.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCVarComponent
{
	GENERATED_BODY()
}; // struct FFlecsCVarComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsCVarComponent, flecs::PairIsTag)