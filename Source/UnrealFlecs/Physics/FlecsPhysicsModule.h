// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "Systems/FlecsSystem.h"
#include "UObject/Object.h"
#include "FlecsPhysicsModule.generated.h"

UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsPhysicsModule : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;

	UPROPERTY()
	FFlecsSystem PhysicsTickSystem;

private:

}; // class UFlecsPhysicsModule
