// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsDefaultEntityEngineSubsystem.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

FFlecsDefaultEntityEngine::FFlecsDefaultEntityEngine()
{
	Initialize();
}

// Initializes the entity engine
void FFlecsDefaultEntityEngine::Initialize()
{
	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	solid_checkf(Settings, TEXT("Default Entities Developer Settings not found"));

	#if WITH_EDITOR
	
	Settings->OnSettingChanged().AddLambda([&](UObject*, FPropertyChangedEvent&)
	{
		RefreshDefaultEntities();
	});
	
	#endif // WITH_EDITOR
	
	// Initial refresh
	RefreshDefaultEntities();
}

// Refreshes the default entities
void FFlecsDefaultEntityEngine::RefreshDefaultEntities()
{
	DefaultEntityWorld = flecs::world();
	
	while (DefaultEntityOptions.IsEmpty())
	{
		TestEntity = DefaultEntityWorld.entity("TestEntity");
		DefaultEntityWorld.progress();
		
		DefaultEntityWorld.query_builder<>()
			.without(TestEntity)
			.with(flecs::Trait).or_()
			.with(flecs::PairIsTag).or_()
			.with_name_component()
			.each([&](flecs::entity Entity)
			{
				if UNLIKELY_IF(Entity.name().length() <= 0)
				{
					return;
				}
				
				UN_LOGF(LogFlecsEntity, Log,
					"Entity: %s", *FString(Entity.name().c_str()));
				DefaultEntityOptions.Add(Entity.name().c_str(), Entity.id());
			});
	}
	
	AddedDefaultEntities.Empty();
	
	for (const FFlecsDefaultMetaEntity& EntityRecord
		: GetDefault<UFlecsDefaultEntitiesDeveloperSettings>()->DefaultEntities)
	{
		if UNLIKELY_IF(EntityRecord.EntityRecord.Name.IsEmpty())
		{
			UN_LOG(LogFlecsEntity, Warning,
				"One of the default entities has an empty name");
			continue;
		}

		AddedDefaultEntities.Add(FFlecsDefaultMetaEntity(EntityRecord.EntityRecord));
	}

	// Add code-specified entities
	for (const FFlecsDefaultMetaEntity& MetaEntity : CodeAddedDefaultEntities)
	{
		AddedDefaultEntities.Add(FFlecsDefaultMetaEntity(MetaEntity.EntityRecord));
	}
	
	DefaultEntityWorld.progress();
	DefaultEntityWorld.set_entity_range(2000, 10000);

	for (const auto& [EntityRecord] : AddedDefaultEntities)
	{
		UN_LOGF(LogFlecsEntity, Log, "Added default entity: %s", *EntityRecord.Name);

		flecs::entity Entity = DefaultEntityWorld.entity(StringCast<ANSICHAR>(*EntityRecord.Name).Get());
		DefaultEntityOptions.Add(*EntityRecord.Name, Entity);
	}
}

// Adds a new default entity
flecs::entity_t FFlecsDefaultEntityEngine::AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity)
{
	CodeAddedDefaultEntities.Add(DefaultEntity);
	RefreshDefaultEntities();
	return DefaultEntityOptions[DefaultEntity.EntityRecord.Name];
}