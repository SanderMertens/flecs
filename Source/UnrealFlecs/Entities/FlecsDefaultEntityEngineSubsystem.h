// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "flecs.h"

struct UNREALFLECS_API FFlecsDefaultEntityEngine final
{
	static FFlecsDefaultEntityEngine& Get()
	{
		static FFlecsDefaultEntityEngine Instance;
		return Instance;
	}
	
public:
	FFlecsDefaultEntityEngine();
	
	void RefreshDefaultEntities();
	
	flecs::entity_t AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity);
	
	TMap<FString, flecs::entity_t> DefaultEntityOptions;
	TArray<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	TArray<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	flecs::world DefaultEntityWorld;
	flecs::entity TestEntity;

private:
	void Initialize();
};

#define DEFINE_DEFAULT_ENTITY_OPTION(EntityName) \
	FFlecsDefaultMetaEntity EntityName##MetaEntity; \
	EntityName##MetaEntity.EntityRecord.Name = TEXT(#EntityName); \
	EntityName = FFlecsDefaultEntityEngine::Get().AddDefaultEntity(EntityName##MetaEntity)

