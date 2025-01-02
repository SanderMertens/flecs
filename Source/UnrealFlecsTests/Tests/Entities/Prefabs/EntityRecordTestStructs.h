// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityRecordTestStructs.generated.h"

USTRUCT()
struct FTestStruct_EntityRecord
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 IntegerValue;
}; // struct FTestStruct_EntityRecord
