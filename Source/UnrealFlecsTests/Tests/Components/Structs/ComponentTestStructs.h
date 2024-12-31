// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentTestStructs.generated.h"

struct FTestComponent_RegisterComponentTest
{
	int32 Value;
}; // struct FTestComponent

USTRUCT()
struct FUStructTestComponent_RegisterComponentTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value;
}; // struct FUStructTestComponent



