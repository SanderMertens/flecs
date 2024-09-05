// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsPhysicsSceneComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPhysicsSceneComponent
{
	GENERATED_BODY()

public:
	FPhysScene* PhysicsScene = nullptr;
}; // struct FFlecsPhysicsSceneComponent

REGISTER_FLECS_COMPONENT_PROPERTIES(FFlecsPhysicsSceneComponent,
	{ flecs::Sparse }, {} )