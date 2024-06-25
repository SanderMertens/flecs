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

	#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditAnywhere, Category = "Flecs | Default Entities")
	bool bIsOptionEntity = false;
	
	#endif // WITH_EDITORONLY_DATA

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsDefaultMetaEntity& Other) const
	{
		return EntityRecord == Other.EntityRecord
		#if WITH_EDITORONLY_DATA
		&& bIsOptionEntity == Other.bIsOptionEntity
		#endif // WITH_EDITORONLY_DATA
		;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsDefaultMetaEntity& Other) const
	{
		return !(*this == Other);
	}

}; // struct FFlecsEntityRecord

UCLASS(BlueprintType, Config = Engine, DefaultConfig, meta = (DisplayName = "Flecs Default Entities Developer Settings"))
class UNREALFLECS_API UFlecsDefaultEntitiesDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Flecs | Default Entities")
	TArray<FFlecsDefaultMetaEntity> DefaultEntities;
	

}; // class UFlecsDefaultEntitiesDeveloperSettings
