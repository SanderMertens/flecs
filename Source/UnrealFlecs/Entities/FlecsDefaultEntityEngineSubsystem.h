// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "flecs.h"

struct UNREALFLECS_API FFlecsDefaultEntityEngine final
{
	static FFlecsDefaultEntityEngine Instance;
	
	static FFlecsDefaultEntityEngine& Get()
	{
		return Instance;
	}

	friend class FUnrealFlecsModule;
	
public:
	FFlecsDefaultEntityEngine();
	~FFlecsDefaultEntityEngine();
	
	flecs::entity_t AddDefaultEntity(FFlecsDefaultMetaEntity DefaultEntity);
	
	TMap<FString, flecs::entity_t> DefaultEntityOptions;
	std::vector<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	std::vector<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	flecs::world* DefaultEntityWorld = nullptr;
	flecs::query<> DefaultEntityQuery;

	bool bIsInitialized = false;
	
	void Initialize();
}; // struct FFlecsDefaultEntityEngine

#define DECLARE_DEFAULT_ENTITY(DefaultEntityName) \
	extern flecs::entity_t DefaultEntityName;

#define DEFINE_DEFAULT_ENTITY(DefaultEntityName, InEntityId) \
	flecs::entity_t DefaultEntityName = InEntityId; \
	namespace \
	{                                                             \
		static void Register##DefaultEntityName()                        \
		{                                                         \
			FFlecsDefaultMetaEntity MetaEntity;                   \
			MetaEntity.EntityName = TEXT(#DefaultEntityName);      \
			MetaEntity.SetId = DefaultEntityName;                      \
			if (!FFlecsDefaultEntityEngine::Get().bIsInitialized)  \
			{                                                     \
				FFlecsDefaultEntityEngine::Get().Initialize();                                   \
			}                                                     \
			(DefaultEntityName) = FFlecsDefaultEntityEngine::Get().AddDefaultEntity(MetaEntity); \
		}                                                         \
		struct FRegisterInvoker##DefaultEntityName                       \
		{                                                         \
			FRegisterInvoker##DefaultEntityName()                        \
			{                                                     \
				FCoreDelegates::OnPostEngineInit.AddStatic(&Register##DefaultEntityName); \
			}                                                     \
		};                                                        \
		inline FRegisterInvoker##DefaultEntityName Invoker##DefaultEntityName;  \
	}

