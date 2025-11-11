// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"

#include "FlecsCollectionDefinition.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSubEntityCollectionReferences
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Flecs")
	TArray<FFlecsCollectionReference> CollectionReferences;
	
}; // struct FFlecsSubEntityCollectionReferences

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Flecs")
	FName Name;
	
	UPROPERTY(EditAnywhere, Category = "Flecs")
	FFlecsEntityRecord Record;

	UPROPERTY(EditAnywhere, Category = "Flecs")
	TArray<FFlecsCollectionReference> Collections;

	UPROPERTY(EditAnywhere, Category = "Flecs")
	TMap<int32 /* Index */, FFlecsSubEntityCollectionReferences> SubEntityCollections;
	
}; // struct FFlecsCollectionDefinition
