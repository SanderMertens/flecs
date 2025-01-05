// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "RegisteredPropertyTest.generated.h"

USTRUCT()
struct FTestRegisteredPropertyStruct_RegisterPropertyTest
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

REGISTER_COMPONENT_TAG_PROPERTIES(FTestRegisteredPropertyStruct_RegisterPropertyTest, flecs::PairIsTag);

USTRUCT()
struct FTestRegisteredTraitProperty_RegisterPropertyTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;
}; // struct FTestRegisteredTraitProperty

USTRUCT()
struct FTestRegisteredPropertyStruct2_RegisterPropertyTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;

	UPROPERTY()
	float MyFloat;

	UPROPERTY()
	FString MyString;
}; // struct FTestRegisteredPropertyStruct2

REGISTER_COMPONENT_TRAIT_PROPERTIES(FTestRegisteredPropertyStruct2_RegisterPropertyTest,
	TRAIT_PROPERTY_STRUCT(FTestRegisteredTraitProperty_RegisterPropertyTest, { 1 } ));

USTRUCT()
struct FTestRegisteredPropertyStruct3_RegisterPropertyTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 MyInt32;

	UPROPERTY()
	float MyFloat;

	UPROPERTY()
	FString MyString;
}; // struct FTestRegisteredPropertyStruct3

REGISTER_COMPONENT_TRAIT_PROPERTIES(FTestRegisteredPropertyStruct3_RegisterPropertyTest,
	TRAIT_PROPERTY_STRUCT(FTestRegisteredTraitProperty_RegisterPropertyTest, { 2 } ));
REGISTER_COMPONENT_TAG_PROPERTIES(FTestRegisteredPropertyStruct3_RegisterPropertyTest, flecs::PairIsTag);
