// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlecsEntityRecord.h"
#include "Collections/FlecsCollectionTypes.h"

#include "FlecsEntityInitializationType.generated.h"

UENUM(BlueprintType)
enum class EFlecsEntityInitializationType : uint8
{
	Record,
	InstancedCollection,
}; // enum class EFlecsEntityInitializationType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityInitializationType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	EFlecsEntityInitializationType Type = EFlecsEntityInitializationType::Record;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Type == EFlecsEntityInitializationType::Record", EditConditionHides))
	FFlecsEntityRecord Record;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
			meta = (EditCondition = "Type == EFlecsEntityInitializationType::InstancedCollection", EditConditionHides))
	FFlecsCollectionInstancedReference InstancedCollection;
	
}; // struct FFlecsEntityInitializationType