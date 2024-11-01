// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsModuleInterface.h"
#include "UObject/Object.h"
#include "FlecsModuleObject.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, ClassGroup = (Flecs))
class UNREALFLECS_API UFlecsModuleObject : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:

}; // class UFlecsModuleObject
