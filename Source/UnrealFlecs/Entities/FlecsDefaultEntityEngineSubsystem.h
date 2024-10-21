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

	friend class FUnrealFlecsModule;
	
public:
	FFlecsDefaultEntityEngine();
	
	void RefreshDefaultEntities();
	
	flecs::entity_t AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity);
	static flecs::entity_t EnqueueDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity);
	
	TMap<FString, flecs::entity_t> DefaultEntityOptions;
	TArray<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	TArray<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	flecs::world DefaultEntityWorld;
	flecs::entity TestEntity;

	bool bIsInitialized = false;

private:
	void Initialize();
};

#define DEFINE_DEFAULT_ENTITY_OPTION(EntityName) \
		INLINE ECS_ENTITY_DECLARE(EntityName); \
		namespace \
		{                                                             \
			struct FRegister##EntityName \
			{                                     \
				FRegister##EntityName() \
				{										 \
					FFlecsDefaultMetaEntity MetaEntity;                        \
					MetaEntity.EntityRecord.Name = TEXT(#EntityName);          \
					if (FFlecsDefaultEntityEngine::Get().bIsInitialized)       \
					{                                                         \
						EntityName = FFlecsDefaultEntityEngine::Get().AddDefaultEntity(MetaEntity); \
					}                                                         \
					else                                                      \
					{                                                         \
						EntityName = FFlecsDefaultEntityEngine::EnqueueDefaultEntity(MetaEntity); \
					}                                                         \
				}                                                               \
			};                                                                  \
			static FRegister##EntityName Register##EntityName;                 \
		}                                                                       \

