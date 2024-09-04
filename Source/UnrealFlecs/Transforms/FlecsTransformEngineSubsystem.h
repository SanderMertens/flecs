// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsTransformEngineSubsystem.generated.h"

INLINE ECS_ENTITY_DECLARE(FlecsLocalTrait);
INLINE ECS_ENTITY_DECLARE(FlecsRelativeTrait);
INLINE ECS_ENTITY_DECLARE(FlecsGlobalTrait);


UCLASS()
class UNREALFLECS_API UFlecsTransformEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFlecsTransformEngineSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

}; // class UFlecsTransformEngineSubsystem
