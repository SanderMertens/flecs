// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlecsModuleProgressInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UFlecsModuleProgressInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsModuleProgressInterface

class UNREALFLECS_API IFlecsModuleProgressInterface
{
	GENERATED_BODY()
public:
	virtual void ProgressModule(double InDeltaTime) = 0;

}; // class IFlecsModuleProgressInterface
