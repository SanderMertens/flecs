// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "FlecsEntityRecord.h"
#include "SolidMacros/Macros.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsDefaultEntityEngineSubsystem.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsDefaultEntityEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFlecsDefaultEntityEngineSubsystem();
	
	struct FFlecsEntityOption
	{
	}; // struct FFlecsEntityOption

	struct FFlecsAddedDefaultEntity
	{
		FFlecsEntityRecord EntityRecord;
	}; // struct FFlecsAddedDefaultEntity
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	void RefreshDefaultEntities();


	TMap<FName, flecs::entity_t> DefaultEntityOptions;
	TArray<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	
}; // class UFlecsDefaultEntityEngineSubsystem

#define REGISTER_FLECS_ENTITY_OPTION(EntityName, EntityHandle) \
	GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->RegisterEntityOption(EntityName, EntityHandle);
