// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityRecord.h"
#include "Engine/DeveloperSettings.h"
#include "SolidMacros/Macros.h"
#include "FlecsDefaultEntitiesDeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FFlecsDefaultMetaEntity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Flecs | Default Entities")
	FFlecsEntityRecord EntityRecord;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsDefaultMetaEntity& Other) const
	{
		return EntityRecord == Other.EntityRecord;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsDefaultMetaEntity& Other) const
	{
		return !(*this == Other);
	}

	FFlecsDefaultMetaEntity() = default;
	FFlecsDefaultMetaEntity(const FFlecsEntityRecord& EntityRecord)
		: EntityRecord(EntityRecord)
	{
	}
	
}; // struct FFlecsEntityRecord

UCLASS(BlueprintType, Config = Flecs, DefaultConfig, Category = "Flecs",
	meta = (DisplayName = "Flecs Default Entities Developer Settings"))
class UNREALFLECS_API UFlecsDefaultEntitiesDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Flecs | Default Entities")
	TArray<FFlecsDefaultMetaEntity> DefaultEntities;

}; // class UFlecsDefaultEntitiesDeveloperSettings
