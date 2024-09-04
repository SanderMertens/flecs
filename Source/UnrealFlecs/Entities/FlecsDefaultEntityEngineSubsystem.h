// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsDefaultEntityEngineSubsystem.generated.h"

INLINE ECS_ENTITY_DECLARE(FlecsNotNetworkedTrait);

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsDefaultEntityEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFlecsDefaultEntityEngineSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	void RefreshDefaultEntities();

	TMap<FString, flecs::entity_t> DefaultEntityOptions;
	TArray<FFlecsDefaultMetaEntity> AddedDefaultEntities;

	TArray<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;

	flecs::entity_t AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity);

	flecs::world DefaultEntityWorld;
	flecs::entity TestEntity;
	
}; // class UFlecsDefaultEntityEngineSubsystem

#if WITH_EDITOR

#define DEFINE_DEFAULT_ENTITY_OPTION(EntityName) \
	FFlecsDefaultMetaEntity EntityName##MetaEntity; \
	EntityName##MetaEntity.EntityRecord.Name = TEXT(#EntityName); \
	EntityName##MetaEntity.bIsOptionEntity = true; \
	EntityName = GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->AddDefaultEntity(EntityName##MetaEntity)

#else // WITH_EDITOR

#define DEFINE_DEFAULT_ENTITY_OPTION(EntityName) \
	FFlecsDefaultMetaEntity EntityName##MetaEntity; \
	EntityName##MetaEntity.EntityRecord.Name = #EntityName; \
	EntityName##MetaEntity.bIsOptionEntity = true; \
	EntityName = GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->AddDefaultEntity(EntityName##MetaEntity);

#endif // WITH_EDITOR
