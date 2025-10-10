// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "FlecsDefaultEntitiesDeveloperSettings.h"

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

	/*
	 * Called in FUnrealFlecsModule::StartupModule
	 **/
	void Initialize();

	flecs::entity CreateDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity, const flecs::world& World);
	
	FFlecsId AddDefaultEntity(FFlecsDefaultMetaEntity DefaultEntity);
	
	TMap<FString, FFlecsId> DefaultEntityOptions;

	robin_hood::unordered_map<int32, Unreal::Flecs::EntityFunctionPtr> DefaultEntityFunctions;
	
	std::vector<FFlecsDefaultMetaEntity> AddedDefaultEntities;
	std::vector<FFlecsDefaultMetaEntity> CodeAddedDefaultEntities;
	
	TUniquePtr<flecs::world> DefaultEntityWorld;
	flecs::query<> DefaultEntityQuery;

	bool bIsInitialized = false;
	
}; // struct FFlecsDefaultEntityEngine

// Should be put in your .h file
#define DECLARE_DEFAULT_ENTITY(DefaultEntityName) \
	extern FFlecsId DefaultEntityName;

// Should be put in your .cpp file
#define DEFINE_DEFAULT_ENTITY(DefaultEntityName, InEntityId, Lambda) \
	FFlecsId DefaultEntityName = static_cast<flecs::id_t>(InEntityId); \
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

