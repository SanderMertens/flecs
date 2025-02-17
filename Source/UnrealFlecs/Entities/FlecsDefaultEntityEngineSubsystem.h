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

	flecs::entity CreateDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity, flecs::world& World);
	
	FFlecsId AddDefaultEntity(FFlecsDefaultMetaEntity DefaultEntity);
	
	TMap<FString, FFlecsId> DefaultEntityOptions;

	robin_hood::unordered_flat_map<int32, UnrealFlecs::EntityFunctionPtr> DefaultEntityFunctions;
	
	std::vector<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	std::vector<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	flecs::world* DefaultEntityWorld = nullptr;
	flecs::query<> DefaultEntityQuery;

	bool bIsInitialized = false;
	
	void Initialize();
}; // struct FFlecsDefaultEntityEngine

#define DECLARE_DEFAULT_ENTITY(DefaultEntityName) \
	extern FFlecsId DefaultEntityName;

#define DEFINE_DEFAULT_ENTITY(DefaultEntityName, InEntityId, Lambda) \
	FFlecsId DefaultEntityName = InEntityId; \
	namespace \
	{                                                             \
		static void Register##DefaultEntityName()                        \
		{                                                         \
			FFlecsDefaultMetaEntity MetaEntity;                   \
			MetaEntity.EntityName = TEXT(#DefaultEntityName);      \
			MetaEntity.EntityId = DefaultEntityName;               \
			MetaEntity.SetId = DefaultEntityName;                      \
			if (!FFlecsDefaultEntityEngine::Get().bIsInitialized)  \
			{                                                     \
				FFlecsDefaultEntityEngine::Get().Initialize();                                   \
			}                                                     \
			FFlecsDefaultEntityEngine::Get().DefaultEntityFunctions.insert_or_assign(InEntityId, Lambda); \
			FFlecsDefaultEntityEngine::Get().AddDefaultEntity(MetaEntity); \
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

