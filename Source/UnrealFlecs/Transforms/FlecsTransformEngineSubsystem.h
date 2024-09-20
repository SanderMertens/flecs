// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsTransformEngineSubsystem.generated.h"

INLINE ECS_ENTITY_DECLARE(FlecsLocalTrait);
INLINE ECS_ENTITY_DECLARE(FlecsGlobalTrait);

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRelativeTrait
{
	GENERATED_BODY()
	
	// How far up the hierarchy to look for the relative entity, 0 is the Parent, 1 is the Parent's Parent, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	int32 RelativeToIndex = 0;
}; // struct FFlecsRelativeComponent

UCLASS()
class UNREALFLECS_API UFlecsTransformEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFlecsTransformEngineSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

}; // class UFlecsTransformEngineSubsystem
