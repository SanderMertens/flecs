// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsPhysicsShapeComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPhysicsShapeComponent
{
	GENERATED_BODY()

public:
	FPhysicsShapeHandle PhysicsShapeHandle;
}; // struct FFlecsPhysicsShapeComponent

