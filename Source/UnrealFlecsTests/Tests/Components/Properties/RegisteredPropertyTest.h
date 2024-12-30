// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "RegisteredPropertyTest.generated.h"

USTRUCT()
struct FTestRegisteredPropertyStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 MyInt32;

	UPROPERTY()
	float MyFloat;

	UPROPERTY()
	FString MyString;
	
}; // struct FTestRegisteredPropertyStruct

REGISTER_COMPONENT_TAG_PROPERTIES(FTestRegisteredPropertyStruct, flecs::PairIsTag);

USTRUCT()
struct FTestRegisteredTraitProperty
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;
}; // struct FTestRegisteredTraitProperty

USTRUCT()
struct FTestRegisteredPropertyStruct2
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;

	UPROPERTY()
	float MyFloat;

	UPROPERTY()
	FString MyString;
}; // struct FTestRegisteredPropertyStruct2

REGISTER_COMPONENT_TRAIT_PROPERTIES(FTestRegisteredPropertyStruct2,
FInstancedStruct::Make<FTestRegisteredTraitProperty>(FTestRegisteredTraitProperty{ 1 }));

USTRUCT()
struct FTestRegisteredPropertyStruct3
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;

	UPROPERTY()
	float MyFloat;

	UPROPERTY()
	FString MyString;
}; // struct FTestRegisteredPropertyStruct3

REGISTER_COMPONENT_TRAIT_PROPERTIES(FTestRegisteredPropertyStruct3, FInstancedStruct::Make<FTestRegisteredTraitProperty>(
	FTestRegisteredTraitProperty{ 2 }));
REGISTER_COMPONENT_TAG_PROPERTIES(FTestRegisteredPropertyStruct3, flecs::PairIsTag);
