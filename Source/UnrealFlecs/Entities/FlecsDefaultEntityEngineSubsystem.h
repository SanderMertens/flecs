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
	
	TMap<FString, flecs::entity_t> DefaultEntityOptions;
	TArray<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	TArray<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	flecs::world DefaultEntityWorld;
	flecs::entity NoneEntity;

	bool bIsInitialized = false;
	
	void Initialize();
}; // struct FFlecsDefaultEntityEngine

#define DEFINE_DEFAULT_ENTITY_OPTION(EntityName) \
	INLINE DLLEXPORT ECS_ENTITY_DECLARE(EntityName); \
	namespace \
	{                                                             \
		static void Register##EntityName()                        \
		{                                                         \
			FFlecsDefaultMetaEntity MetaEntity;                   \
			MetaEntity.EntityRecord.Name = TEXT(#EntityName);      \
			if (!FFlecsDefaultEntityEngine::Get().bIsInitialized)  \
			{                                                     \
				FFlecsDefaultEntityEngine::Get().Initialize();                                   \
			}                                                     \
			EntityName = FFlecsDefaultEntityEngine::Get().AddDefaultEntity(MetaEntity); \
		}                                                         \
		struct FRegisterInvoker##EntityName                       \
		{                                                         \
			FRegisterInvoker##EntityName()                        \
			{                                                     \
				FCoreDelegates::OnPostEngineInit.AddStatic(&Register##EntityName); \
			}                                                     \
		};                                                        \
		inline FRegisterInvoker##EntityName Invoker##EntityName;  \
	}

