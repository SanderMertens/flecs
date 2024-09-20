// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "SolidMacros/Macros.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsTickerEngineSubsystem.generated.h"

INLINE ECS_ENTITY_DECLARE(FlecsFixedTick);

UCLASS()
class UNREALFLECS_API UFlecsTickerEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFlecsTickerEngineSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


}; // class UFlecsTickerEngineSubsystem
