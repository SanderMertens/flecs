// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Worlds/FlecsAbstractWorldSubsystem.h"
#include "FlecsTransformWorldSubsystem.generated.h"

UCLASS()
class UNREALFLECS_API UFlecsTransformWorldSubsystem final : public UFlecsAbstractWorldSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;


}; // class UFlecsTransformWorldSubsystem
