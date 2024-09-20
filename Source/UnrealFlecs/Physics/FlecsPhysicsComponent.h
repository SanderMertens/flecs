// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsPhysicsComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPhysicsComponent
{
	GENERATED_BODY()

public:
	FPhysicsActorHandle PhysicsActorHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	FBodyInstance BodyInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	uint8 bSimulatePhysics : 1 = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	uint8 bEnableGravity : 1 = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	uint8 bStatic : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	uint8 bUpdateKinematicFromSimulation : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Physics")
	uint8 bStartAwake : 1 = true;
	
}; // struct FFlecsPhysicsComponent
