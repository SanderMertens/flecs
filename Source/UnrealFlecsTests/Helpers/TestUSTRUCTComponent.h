// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Properties/FlecsComponentProperties.h"
#include "TestUSTRUCTComponent.generated.h"

USTRUCT()
struct FTestUSTRUCTComponent
{
	GENERATED_BODY()

public:

	UPROPERTY()
	float TestFloat = 0.0f;

	UPROPERTY()
	int32 TestInt = 0;

	UPROPERTY()
	FString TestString = TEXT("Test");
	
}; // struct FTestUSTRUCTComponent

