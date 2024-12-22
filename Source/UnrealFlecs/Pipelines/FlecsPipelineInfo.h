// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "UObject/Object.h"
#include "FlecsPipelineInfo.generated.h"

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class UNREALFLECS_API UFlecsPipelineInfo : public UObject, public IFlecsEntityInterface
{
	GENERATED_BODY()

public:

}; // class UFlecsPipelineInfo
