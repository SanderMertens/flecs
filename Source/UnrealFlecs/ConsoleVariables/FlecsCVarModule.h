// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleObject.h"
#include "FlecsCVarModule.generated.h"


UCLASS()
class UNREALFLECS_API UFlecsCVarModule : public UFlecsModuleObject
{
	GENERATED_BODY()

public:
	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity) override;
	virtual void DeinitializeModule(UFlecsWorld* InWorld) override;


}; // class UFlecsCVarModule
